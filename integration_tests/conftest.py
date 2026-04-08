import pytest
import subprocess
import time
import requests
import psutil
import signal
import os
from pathlib import Path

# Configuration
SERVER_EXECUTABLE = Path(__file__).parent.parent / "webserv"
SERVER_CONFIG = Path(__file__).parent.parent / "default.conf"
SERVER_HOST = "127.0.0.1"
SERVER_PORT = 8080
SERVER_URL = f"http://{SERVER_HOST}:{SERVER_PORT}"

@pytest.fixture(scope="session")
def server():
	"""
      Fixture that runs the server for the entire test session.
      Server starts once before all tests and stops after.
	"""
	if not SERVER_EXECUTABLE.exists():
		pytest.fail(f"Server executable not found: {SERVER_EXECUTABLE}")

	# Start server in background
	process = subprocess.Popen(
		[str(SERVER_EXECUTABLE), str(SERVER_CONFIG)],
		stdout=subprocess.PIPE,
		stderr=subprocess.PIPE,
		preexec_fn=os.setsid  # Create new process group
	)

	# Wait for server to be ready
	wait_for_server()

	yield process  # Server is running, run tests

	# Stop server after tests finish
	cleanup_server(process)

@pytest.fixture
def server_url(server):
	"""Server URL for use in tests"""
	return SERVER_URL

def wait_for_server(timeout=5):
	"""Wait until server becomes available"""
	start_time = time.time()
	while time.time() - start_time < timeout:
		try:
			response = requests.get(f"{SERVER_URL}/", timeout=0.5)
			if response.status_code == 200:
				return
		except requests.exceptions.ConnectionError:
			time.sleep(0.1)
	pytest.fail(f"Server failed to start within {timeout} seconds")

def cleanup_server(process):
	"""Safely stop the server"""
	try:
		if psutil.pid_exists(process.pid):
			# Send SIGTERM to process group
			os.killpg(os.getpgid(process.pid), signal.SIGTERM)
			process.wait(timeout=5)
	except (subprocess.TimeoutExpired, ProcessLookupError):
		# Force kill if SIGTERM didn't work
		try:
			os.killpg(os.getpgid(process.pid), signal.SIGKILL)
		except ProcessLookupError:
			pass  # Process already terminated

@pytest.fixture
def client():
	"""Requests client with pre-configured timeout"""
	session = requests.Session()
	session.timeout = 5  # Default timeout for all requests
	return session
