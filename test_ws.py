import asyncio
import websockets

async def test_connection():
    uri = "ws://localhost:5034/notifications"
    print(f"Connecting to {uri}...")
    try:
        async with websockets.connect(uri) as websocket:
            print("Connected!")
            await websocket.send("Hello Server")
            print("Sent: Hello Server")
            response = await websocket.recv()
            print(f"Received: {response}")
    except Exception as e:
        print(f"Connection failed: {e}")

if __name__ == "__main__":
    asyncio.run(test_connection())
