import pika
from google.oauth2 import service_account
from googleapiclient.discovery import build
import os, json, uuid, io, re, sys
from io import BytesIO
from googleapiclient.http import MediaIoBaseDownload
from googleapiclient.http import MediaFileUpload
from PIL import Image
import pytesseract
import fitz  # PyMuPDF
import requests
from datetime import datetime


SCOPES = ['https://www.googleapis.com/auth/drive']
SERVICE_ACCOUNT_FILE = 'service_account.json'

# RabbitMQ configuration from your settings
rabbitmq_url = "amqps://vedfplxy:ULj-Mt6eUZpfm4g489PQxAgHu6-iqJqJ@cow.rmq2.cloudamqp.com/vedfplxy"
queue_name = "ocr_doc_ingestion"
exchange_type = "amq.topic"

# Parse the URL to create connection parameters
params = pika.URLParameters(rabbitmq_url)

# Establish connection to RabbitMQ
connection = pika.BlockingConnection(params)
channel = connection.channel()

# Declare the queue (ensure it exists)
channel.queue_declare(queue=queue_name, durable=True)

# Bind the queue to the exchange (optional if already bound on the RabbitMQ server side)
channel.queue_bind(exchange=exchange_type, queue=queue_name)

def auth_drive():
    """
    Authenticate using service account credentials for automation.
    No user interaction required and credentials don't expire.
    """
    try:
        if not os.path.exists(SERVICE_ACCOUNT_FILE):
            raise FileNotFoundError(f"Service account file '{SERVICE_ACCOUNT_FILE}' not found")

        credentials = service_account.Credentials.from_service_account_file(
            SERVICE_ACCOUNT_FILE,
            scopes=SCOPES
        )

        print("Successfully authenticated with service account")
        return credentials

    except Exception as e:
        print(f"Error authenticating with service account: {e}")
        sys.exit(1)

# Function to upload thumbnail to Google Drive using image file path
def upload_thumbnail(image_path, folder_id):
    creds = auth_drive()  # Assuming `auth_drive` is a method that returns authenticated credentials
    service = build('drive', 'v3', credentials=creds)
    
    # Prepare the file metadata and media upload
    file_metadata = {
        'name': image_path,
        'parents': [folder_id]
    }
    
    # Use image_path to upload the file directly
    media = MediaFileUpload(image_path, mimetype='image/jpeg')
    
    # Create the file on Google Drive and return the file ID
    file = service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    
    return file.get('id')

# Function to upload individual PDF page as a separate PDF to Google Drive
def upload_pdf_page(page, file_id, page_number, folder_id):
    creds = auth_drive()
    service = build('drive', 'v3', credentials=creds)
    
    # Create a new PDF document with just this page
    new_doc = fitz.open()  # Create a new empty PDF
    new_doc.insert_pdf(page.parent, from_page=page.number, to_page=page.number)
    
    # Save the single page PDF to a temporary file
    page_pdf_path = f"{file_id}_page_{page_number}.pdf"
    new_doc.save(page_pdf_path)
    new_doc.close()
    
    # Prepare the file metadata
    file_metadata = {
        'name': page_pdf_path,
        'parents': [folder_id]
    }
    
    # Upload the PDF page to Google Drive
    media = MediaFileUpload(page_pdf_path, mimetype='application/pdf')
    file = service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    
    # Clean up the temporary file
    os.remove(page_pdf_path)
    
    return file.get('id')

# Function to download file using file_id
def download_file(service, file_id, file_name):
     
    request = service.files().get_media(fileId=file_id)
    fh = io.BytesIO()
    downloader = MediaIoBaseDownload(fh, request)
    done = False
    while done is False:
        _, done = downloader.next_chunk()
    with open(file_name, 'wb') as f:
        fh.seek(0)
        f.write(fh.read())
    print(f"Downloaded {file_name}")


# Function to report progress to the API
def report_progress(job_id, progress_percentage):
    payload = {
        "Id": job_id,
        "ProgressPercentage": progress_percentage
    }
    
    try:
        response = requests.post('https://archive.graphic.com.gh/api/services/app/IngestionJob/ReportProgress', json=payload)
        if response.status_code == 200:
            print(f"Progress reported: {progress_percentage}% for job {job_id}")
        else:
            print(f"Failed to report progress: {response.status_code} - {response.content}")
    except Exception as e:
        print(f"Error reporting progress: {str(e)}")


# Function to extract text from PDF using fitz (PyMuPDF) and handle API request
def process_pdf(job_id, file_path, file_id):
    # Open the PDF file
    doc = fitz.open(file_path)
    
    document_url = f'https://lh3.googleusercontent.com/d/{file_id}'
    thumbnail_folder_id = "1cOjRx4te1hWnXBZhCpFEDBVegEQfIKta"
    split_document_folder_id = "19a8O7xCJKmzUQ_aGvAftBjpBhbprV6v1"
    
    total_pages = len(doc)
    
    for page_number in range(total_pages):
        page = doc.load_page(page_number)  # Load each page
        page_text = page.get_text("text")  # Extract text from the page

        # Convert the page to an image and save it as a PNG
        page_image = page.get_pixmap()  # Get an image of the page
        page_image_path = f"{file_id}_page_{page_number + 1}.png"
        page_image.save(page_image_path)

        # Upload the image to Google Drive (thumbnail)
        thumbnail_id = upload_thumbnail(page_image_path, thumbnail_folder_id)
        thumbnail_url = f'https://lh3.googleusercontent.com/d/{thumbnail_id}'

        # Upload the individual PDF page to the split document folder
        page_pdf_id = upload_pdf_page(page, file_id, page_number + 1, split_document_folder_id)
        page_document_url = f'https://lh3.googleusercontent.com/d/{page_pdf_id}'

        # Clean up the temporary image file
        os.remove(page_image_path)

        # Prepare the API request payload for the page
        payload = {
            "Id": job_id,
            "PublicationText": page_text,
            "FileType": "pdf",
            "ThumbnailUrl": thumbnail_url,
            "DocumentUrl": page_document_url,  # Use the individual page PDF URL
            "PageNumber": page_number + 1,
            "Categories": [],
            "Tags": [],
            "Titles" : []
        }

        # Make API request to send data for the page
        response = requests.post('https://archive.graphic.com.gh/api/services/app/Publication/IngestDocument', json=payload)
        if response.status_code != 200:
            print(f"Failed to send data for page {page_number + 1}: {response.content}")
        else:
            print(f"Successfully processed page {page_number + 1}")

        # Calculate and report progress percentage
        progress_percentage = round(((page_number + 1) / total_pages) * 100, 2)
        report_progress(job_id, progress_percentage)

    # Close the PDF file
    doc.close()

    # Call an API to set the job as completed
    job_report_response = requests.post('https://archive.graphic.com.gh/api/services/app/IngestionJob/UpdateStatus', json={"JobId": job_id , "Status": "Completed"})
    if job_report_response.status_code != 200:
        print(f"Job completion report not sent: job_id {job_id}")
    else:
        print(f"Successfully reported job status: job_id {job_id}")



def process_image(job_id, file_path, file_id):
    try:
        # Open the image file
        image = Image.open(file_path)
        
        document_url = f'https://lh3.googleusercontent.com/d/{file_id}'
        thumbnail_folder_id = "1cOjRx4te1hWnXBZhCpFEDBVegEQfIKta"

        # Extract text from the image
        extracted_text = pytesseract.image_to_string(image)
        
        # Clean text more appropriately - remove excessive whitespace and control characters
        # but preserve alphanumeric characters, basic punctuation, and spacing
        clean_text = re.sub(r'\s+', ' ', extracted_text.strip())
        clean_text = re.sub(r'[^\w\s.,!?;:()\-"\'$%&+=/]', '', clean_text)
        
        # Validate that we have meaningful text
        if not clean_text or len(clean_text.strip()) < 3:
            print(f"Warning: No meaningful text extracted from image {file_id}")
            clean_text = "No text could be extracted from this image"

        # Create thumbnail file path
        thumbnail_file_path = f"{file_id}_thumbnail.png"
        
        # Convert and save the image as PNG for thumbnail
        image.save(thumbnail_file_path, "PNG")

        # Upload the image to Google Drive
        thumbnail_id = upload_thumbnail(thumbnail_file_path, thumbnail_folder_id)
        thumbnail_url = f'https://lh3.googleusercontent.com/d/{thumbnail_id}'

        # Clean up the temporary thumbnail file
        if os.path.exists(thumbnail_file_path):
            os.remove(thumbnail_file_path)

        # Prepare the API request payload for the image
        payload = {
            "Id": job_id,
            "PublicationText": clean_text,
            "FileType": "image",
            "ThumbnailUrl": thumbnail_url,
            "DocumentUrl": document_url,
            "PageNumber": 1,
            "Categories": [],
            "Tags": [],
            "Titles": []
        }

        # Make API request to send data for the image
        try:
            response = requests.post(
                'https://archive.graphic.com.gh/api/services/app/Publication/IngestDocument', 
                json=payload,
                timeout=30  # Add timeout
            )
            
            if response.status_code != 200:
                print(f"Failed to send data for image: {response.status_code} - {response.content}")
                raise Exception(f"API request failed with status {response.status_code}")
            else:
                print(f"Successfully processed image")
                
        except requests.exceptions.RequestException as e:
            print(f"Error making API request: {str(e)}")
            raise

        # Report 100% progress for single image processing
        report_progress(job_id, 100.0)

        # Call an API to set the job as completed
        try:
            job_report_response = requests.post(
                'https://archive.graphic.com.gh/api/services/app/IngestionJob/UpdateStatus', 
                json={"JobId": job_id, "Status": "Completed"},
                timeout=30
            )
            
            if job_report_response.status_code != 200:
                print(f"Job completion report not sent: job_id {job_id} - {job_report_response.content}")
            else:
                print(f"Successfully reported job status: job_id {job_id}")
                
        except requests.exceptions.RequestException as e:
            print(f"Error reporting job completion: {str(e)}")

    except Exception as e:
        print(f"Error processing image {file_id}: {str(e)}")
        
        # Try to report job as failed
        try:
            failure_response = requests.post(
                'https://archive.graphic.com.gh/api/services/app/IngestionJob/UpdateStatus', 
                json={"JobId": job_id, "Status": "Failed"},
                timeout=30
            )
            if failure_response.status_code == 200:
                print(f"Successfully reported job failure: job_id {job_id}")
            else:
                print(f"Failed to report job failure: job_id {job_id}")
        except:
            print(f"Could not report job failure for job_id {job_id}")
        
        # Re-raise the exception to let the caller handle it
        raise




# Define the callback function for message consumption
def callback(ch, method, properties, body):
    print(f"Received message: {body}")
    # Acknowledge the message to RabbitMQ that it has been processed
    ch.basic_ack(delivery_tag=method.delivery_tag)

    # convert the message to a json object
    message_object = json.loads(body.decode('utf-8'))

    # Get the file ID and name from the message
    file_id = message_object['FileId']
    job_id = message_object['IngestionJobId']
    file_type = message_object['FileType']
    file_name = message_object['FileName']

    creds = auth_drive()
    service = build('drive', 'v3', credentials=creds)

    # Download the file
    download_file(service, file_id, file_name)

    # Check file type and proceed accordingly
    if file_type == "pdf":
        process_pdf(job_id, file_name, file_id)
    else:
        process_image(job_id, file_name, file_id)



# Start consuming messages from the queue
channel.basic_consume(queue=queue_name, on_message_callback=callback)

print(f"Waiting for messages in archive_ingestion_engine. To exit press CTRL+C")
channel.start_consuming()