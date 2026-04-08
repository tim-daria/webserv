
class TestServerBasics:
	"""Basic server health tests"""

	def test_server_responds_to_root(self, server_url, client):
		"""Server should respond to GET request to root"""
		response = client.get(f"{server_url}/")
		assert response.status_code == 200
		assert "Hello webserv!" in response.text


class TestServerHTTPMethods:
	"""Tests for different HTTP methods"""

	def test_post_request(self, server_url, client):
		response = client.post(f"{server_url}/", data={"key": "value"})
		# Your server should handle POST or return 405 Method Not Allowed
		assert response.status_code in [200, 405]


class TestServerEdgeCases:
	"""Edge case and error handling tests"""

	def test_multiple_concurrent_requests(self, server_url, client):
		"""Server should handle multiple concurrent requests"""
		import threading

		results = []
		errors = []

		def make_request():
			try:
				response = client.get(f"{server_url}/")
				results.append(response.status_code)
			except Exception as e:
				errors.append(str(e))

		# Launch 10 parallel requests
		threads = [threading.Thread(target=make_request) for _ in range(10)]
		for t in threads:
			t.start()
		for t in threads:
			t.join()

		assert len(errors) == 0, f"Errors: {errors}"
		assert len(results) == 10
		assert all(code == 200 for code in results)
