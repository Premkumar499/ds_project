# Library Book Finder - Data Structures Project

A web application demonstrating efficient book search using data structures concepts.

## 🚀 Live Demo
Deployed on Vercel: [Your deployment URL will appear here after deployment]

## 🎯 Features
- **Book Search**: Search books by ID for instant results
- **Responsive Design**: Works on desktop and mobile devices
- **Data Structures**: Originally implements hashmap for efficient search (C backend)
- **Web Deployment**: Frontend deployed to Vercel using JSON-based search

## 🛠️ Technology Stack
- **Frontend**: HTML5, CSS3, JavaScript (ES6+)
- **Backend**: C (hashmap implementation) - for local development
- **Deployment**: Vercel (static hosting)
- **Data**: JSON database of books

## 📱 How to Use
1. Enter a Book ID in the search box
2. Click "Search" or press Enter
3. View the book details including title, author, genre, and description

## 🏗️ Local Development
To run locally with full hashmap backend:
```bash
./run_system.sh
```

## 📦 Deployment
This project is optimized for Vercel deployment with automatic fallback to JSON search when the C backend is not available.

## 📊 Project Structure
```
├── index.html          # Main HTML file
├── style.css           # Styling
├── script.js           # JavaScript logic
├── books.json          # Book database
├── backend/            # C implementation
│   └── library_hashmap.c
└── frontend/           # Original frontend files
```