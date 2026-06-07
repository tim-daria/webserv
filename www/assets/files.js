// Shared helpers for the upload and delete pages.
// Each block is guarded so it only runs when the elements it needs exist,
// which means the delete logic stays inert on the upload page and vice versa.

(function () {
  const result = document.getElementById("result");

  // Turn a raw byte count into a short, readable string (e.g. "82 KB").
  function formatSize(bytes) {
    const n = Number(bytes);
    if (!Number.isFinite(n)) return bytes;
    if (n < 1024) return n + " B";
    if (n < 1024 * 1024) return (n / 1024).toFixed(1) + " KB";
    return (n / (1024 * 1024)).toFixed(1) + " MB";
  }

  function showResult(type, message) {
    if (!result) return;
    result.className = "result " + type;
    result.textContent = message;
  }

  // Fetches the autoindex page for /uploads/ and renders a clean list.
  // Exposed on window so other inline handlers (e.g. upload onload) can refresh it.
  async function loadFileList() {
    const list = document.getElementById("file-list");
    if (!list) return; // page has no list -> nothing to do
    try {
      const res = await fetch("/uploads/");
      if (!res.ok) throw new Error("Failed to fetch");
      const html = await res.text();
      const doc = new DOMParser().parseFromString(html, "text/html");
      const links = [...doc.querySelectorAll("pre a")].filter(
        (a) => a.getAttribute("href") !== "../",
      );
      if (links.length === 0) {
        list.innerHTML = '<li class="empty">No files uploaded yet.</li>';
        return;
      }
      list.innerHTML = links
        .map((a) => {
          const name = a.textContent.trim();
          // The autoindex prints "NAME   SIZE\n", so this row's size is
          // the text node right after the link (up to the newline), not the
          // whole <pre> block.
          const next = a.nextSibling;
          const sizeText =
            next && next.nodeType === Node.TEXT_NODE
              ? next.textContent.trim().split(/\s+/)[0]
              : "";
          return (
            "<li>" +
            '<span class="file-name">' +
            '<a href="/uploads/' +
            name +
            '">' +
            name +
            "</a>" +
            "</span>" +
            '<span class="file-size">' +
            formatSize(sizeText) +
            "</span>" +
            "</li>"
          );
        })
        .join("");
      // Column headers above the rows.
      list.innerHTML =
        '<li class="file-list-head">' +
        '<span class="file-name">Name</span>' +
        '<span class="file-size">Size</span>' +
        "</li>" +
        list.innerHTML;
    } catch (e) {
      list.innerHTML = '<li class="empty">Could not load file list.</li>';
    }
  }
  window.loadFileList = loadFileList;

  async function deleteFile(path) {
    showResult("", 'Deleting "' + path + '"\u2026');
    try {
      const res = await fetch(path, { method: "DELETE" });
      if (res.ok) {
        showResult("success", 'Deleted "' + path + '" successfully.');
        loadFileList();
      } else {
        showResult("error", "Delete failed (HTTP " + res.status + ").");
      }
    } catch (e) {
      showResult("error", "Delete failed: could not reach the server.");
    }
  }

  // helper function for file upload.
  async function handleUpload(event) {
    event.preventDefault();
    const fileInput = document.getElementById("file");
    if (!fileInput || !fileInput.files.length) {
      showResult("error", "Please choose a file first.");
      return;
    }

    showResult("", "Uploading\u2026");
    const data = new FormData(uploadForm);

    try {
      const res = await fetch("/upload", { method: "POST", body: data });
      if (res.ok || res.status === 201) {
        showResult("success", "File uploaded successfully.");
        uploadForm.reset();
        loadFileList();
      } else {
        showResult("error", "Upload failed (HTTP " + res.status + ").");
      }
    } catch (e) {
      showResult("error", "Upload failed: could not reach the server.");
    }
  }

  // Only wire up the upload form if this page has one.
  const uploadForm = document.querySelector("form[action='/upload']");
  if (uploadForm) {
    uploadForm.addEventListener("submit", handleUpload(event));
  }

  // Only wire up the delete form if this page actually has one.
  const deleteForm = document.getElementById("delete-form");
  if (deleteForm) {
    deleteForm.addEventListener("submit", function (event) {
      // Stop the browser from doing a default GET form submit; send a real
      // HTTP DELETE request to the file's path instead.
      event.preventDefault();
      let path = document.getElementById("filepath").value.trim();
      if (!path) {
        showResult("error", "Please enter a file path.");
        return;
      }
      if (path.charAt(0) !== "/") path = "/" + path;
      deleteFile(path);
    });
  }

  // Render the list on load (no-op if there's no #file-list).
  loadFileList();
})();
