async function searchBook() {
  const id = document.getElementById("searchInput").value.trim();
  const resultDiv = document.getElementById("result");
  
  // Clear result if no input
  if (!id) {
    resultDiv.innerHTML = "";
    return;
  }
  
  resultDiv.innerHTML = "<p>🔍 Searching...</p>";

  try {
    const response = await fetch("books.json");
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);

    const books = await response.json();
    const book = books.find(b => b.id == id);

    if (book) {
      resultDiv.innerHTML = `
        <div class="book-card">
          <div class="book-image-container">
            <img src="${book.image}" alt="${book.title}" onload="this.style.opacity=1; this.style.display='block'; this.nextElementSibling.style.display='none';" onerror="this.style.display='none'; this.nextElementSibling.style.display='flex';" style="opacity:0; transition: opacity 0.5s ease-in-out;">
            <div class="image-placeholder">
              <span>📚</span>
              <p>Book Cover</p>
              <small>${book.title}</small>
            </div>
          </div>
          <div class="book-info">
            <h2 class="book-title">${book.title}</h2>
            <div class="book-details">
              <p class="book-author"><strong>✍️ Author:</strong> ${book.author}</p>
              <p class="book-genre"><strong>📚 Genre:</strong> ${book.genre}</p>
              <p class="book-year"><strong>📅 Published:</strong> ${book.year}</p>
            </div>
            <div class="book-description">
              <h3>📖 Description</h3>
              <p>${book.description}</p>
            </div>
          </div>
        </div>
      `;
    } else {
      resultDiv.innerHTML = `<p class="error">❌ Book not found. Try another ID.</p>`;
    }
  } catch (error) {
    resultDiv.innerHTML = `<p class="error">⚠️ Error: ${error.message}</p>`;
  }
}

// Add Enter key support
document.addEventListener('DOMContentLoaded', function() {
  const searchInput = document.getElementById("searchInput");
  searchInput.addEventListener('keypress', function(event) {
    if (event.key === 'Enter') {
      searchBook();
    }
  });
});
