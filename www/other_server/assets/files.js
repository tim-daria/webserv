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

  // Generic form handler.
  // type: "upload" | "contact" | "delete"
  function makeFormHandler(type, form) {
    return async function (event) {
      event.preventDefault();

      let url, method, body, headers, pending, success, fail;

      if (type === "upload") {
        const fileInput = document.getElementById("file");
        if (!fileInput || !fileInput.files.length) {
          showResult("error", "Please choose a file first.");
          return;
        }
        url = form.action;
        method = "POST";
        body = new FormData(form);
        headers = {};
        pending = "Uploading\u2026";
        success = "File uploaded successfully.";
        fail = "Upload failed";
      } else if (type === "contact") {
        url = form.action;
        method = "POST";
        body = new URLSearchParams(new FormData(form)).toString();
        headers = { "Content-Type": "application/x-www-form-urlencoded" };
        pending = "Sending\u2026";
        success = "Message sent successfully.";
        fail = "Submission failed";
      } else if (type === "delete") {
        let path = document.getElementById("filepath").value.trim();
        if (!path) {
          showResult("error", "Please enter a file path.");
          return;
        }
        if (path.charAt(0) !== "/") path = "/" + path;
        url = path;
        method = "DELETE";
        body = null;
        headers = {};
        pending = 'Deleting "' + path + '"\u2026';
        success = 'Deleted "' + path + '" successfully.';
        fail = "Delete failed";
      }

      showResult("", pending);
      try {
        const res = await fetch(url, {
          method: method,
          headers: headers,
          body: body,
        });
        if (res.ok || res.status === 201) {
          showResult("success", success);
          form.reset();
          if (type === "upload" || type === "delete") loadFileList();
        } else {
          showResult("error", fail + " (HTTP " + res.status + ").");
        }
      } catch (e) {
        showResult("error", fail + ": could not reach the server.");
      }
    };
  }

  // Only wire up the upload form if this page has one.
  const uploadForm = document.querySelector("form[action='/upload']");
  if (uploadForm) {
    uploadForm.addEventListener(
      "submit",
      makeFormHandler("upload", uploadForm),
    );
  }

  // Only wire up the contact/submit form if this page has one.
  const contactForm = document.getElementById("contact-form");
  if (contactForm) {
    contactForm.addEventListener(
      "submit",
      makeFormHandler("contact", contactForm),
    );
  }

  // Only wire up the delete form if this page actually has one.
  const deleteForm = document.getElementById("delete-form");
  if (deleteForm) {
    deleteForm.addEventListener(
      "submit",
      makeFormHandler("delete", deleteForm),
    );
  }

  // Render the list on load (no-op if there's no #file-list).
  loadFileList();
})();
