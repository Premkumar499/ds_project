#!/bin/bash

# Library Management System - Auto Run Script
# This script automatically starts both backend and frontend
# Backend uses hashmap for efficient search, frontend shows search method

clear
echo "================================"
echo "Library Management System"
echo "Auto-Starting Backend & Frontend"
echo "Using Hashmap + JSON Integration"
echo "================================"
echo ""

# Function to cleanup background processes
cleanup() {
    echo ""
    echo "🛑 Shutting down services..."
    kill $FRONTEND_PID 2>/dev/null
    kill $BACKEND_PID 2>/dev/null
    echo "✅ Services stopped."
    exit 0
}

# Trap Ctrl+C to cleanup
trap cleanup SIGINT

# Start Backend API Server
echo "🔧 Starting Backend Hashmap API..."
cd backend
# Compile if needed
if [ ! -f "library_hashmap" ] || [ "library_hashmap.c" -nt "library_hashmap" ]; then
    echo "📦 Compiling backend..."
    gcc -o library_hashmap library_hashmap.c -lcjson
fi

./library_hashmap > /dev/null 2>&1 &
BACKEND_PID=$!
cd ..

# Give backend time to start
sleep 3

# Check if backend started successfully
if kill -0 $BACKEND_PID 2>/dev/null; then
    echo "✅ Backend API running at http://localhost:8080"
else
    echo "❌ Failed to start backend server"
    exit 1
fi

# Start Frontend Web Server
echo "🌐 Starting Frontend Web Server..."
cd frontend
python3 -m http.server 8000 > /dev/null 2>&1 &
FRONTEND_PID=$!
cd ..

# Give frontend time to start
sleep 2

# Check if frontend started successfully
if kill -0 $FRONTEND_PID 2>/dev/null; then
    echo "✅ Frontend running at http://localhost:8000"
else
    echo "❌ Failed to start frontend server"
    kill $BACKEND_PID 2>/dev/null
    exit 1
fi

echo ""
echo "🔗 Services Running:"
echo "   Backend API: http://localhost:8080 (Hashmap search)"
echo "   Frontend:    http://localhost:8000 (Smart search UI)"
echo "   JSON Data:   frontend/books.json (100 books)"
echo ""
echo "� Features:"
echo "   • O(1) hashmap search via backend API"
echo "   • Fallback to O(n) JSON search if backend unavailable"
echo "   • Visual indicators showing which search method was used"
echo "   • Hash bucket information for educational purposes"
echo ""
echo "🧪 Test the hashmap:"
echo "   curl http://localhost:8080/search/101"
echo "   curl http://localhost:8080/health"
echo ""
echo "Press Ctrl+C to stop all services"
echo "================================"

# Keep script running and monitor services
while true; do
    if ! kill -0 $FRONTEND_PID 2>/dev/null; then
        echo "❌ Frontend server stopped unexpectedly"
        kill $BACKEND_PID 2>/dev/null
        break
    fi
    if ! kill -0 $BACKEND_PID 2>/dev/null; then
        echo "⚠️  Backend server stopped unexpectedly (frontend will use JSON fallback)"
    fi
    sleep 5
done
