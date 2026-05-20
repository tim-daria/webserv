import requests

class TestServerBasics:
    """Basic server health tests"""

    def test_server_responds_to_root(self, server_url, client):
        response = client.get(f"{server_url}/")

        assert response.status_code == 200

    def test_server_returns_content(self, server_url, client):
        response = client.get(f"{server_url}/")

        assert len(response.text) > 0


class TestGETRequests:
    """GET request integration tests"""

    def test_get_existing_html_file(self, server_url, client):
        response = client.get(f"{server_url}/index.html")

        assert response.status_code == 200
        assert "html" in response.text.lower()

    # def test_get_existing_text_file(self, server_url, client):
    #     response = client.get(f"{server_url}/test.txt")

    #     assert response.status_code == 200
    #     assert len(response.text) > 0

    # def test_get_nonexistent_file(self, server_url, client):
    #     response = client.get(f"{server_url}/does_not_exist.txt")

    #     assert response.status_code == 404

    # def test_get_empty_file(self, server_url, client):
    #     response = client.get(f"{server_url}/empty.txt")

    #     assert response.status_code == 200
    #     assert response.text == ""

    # def test_get_large_file(self, server_url, client):
    #     response = client.get(f"{server_url}/large.txt")

    #     assert response.status_code == 200
    #     assert len(response.content) > 100000

    def test_binary_file(self, server_url, client):
        response = client.get(f"{server_url}/uploads/cat.jpg")

        assert response.status_code == 200
        assert response.content[:4] == b"\xff\xd8\xff\xe0"  # Response contains a JPEG image


class TestHTTPMethods:
    """HTTP method tests"""

    def test_post_request(self, server_url, client):
        response = client.post(
            f"{server_url}/",
            data={"key": "value"}
        )

        assert response.status_code in [200, 405]

    def test_delete_request(self, server_url, client):
        response = client.delete(f"{server_url}/")

        assert response.status_code in [200, 405]

    def test_put_request_returns_501(self, server_url, client):
        """PUT is not implemented by this server"""
        response = client.put(f"{server_url}/")

        assert response.status_code == 501

    def test_get_on_delete_only_route_returns_405(self, server_url, client):
        """GET is a known method but not allowed on /method_not_allowed"""
        response = client.get(f"{server_url}/method_not_allowed")

        assert response.status_code == 405


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


# =========================================================
# OPTIONAL ADVANCED TESTS
# =========================================================


class TestAdvancedCases:
    """Advanced edge cases"""

    def test_query_string(self, server_url, client):
        response = client.get(
            f"{server_url}/index.html?foo=bar"
        )

        assert response.status_code in [200, 404]

    def test_spaces_in_url(self, server_url, client):
        response = client.get(
            f"{server_url}/hello%20world.txt"
        )

        assert response.status_code in [200, 404]

    def test_very_long_url(self, server_url, client):
        long_path = "/" + "a" * 2000

        response = client.get(
            f"{server_url}{long_path}"
        )

        assert response.status_code in [404, 414]
