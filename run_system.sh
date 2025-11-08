#!/bin/bash

# Library Management System - Auto Run Script
# This script automatically starts both backend and frontend
# Now using unified JSON file for both services

clear
echo "================================"
echo "Library Management System"
echo "Auto-Starting Backend & Frontend"
echo "Using Shared JSON Database"
echo "================================"
echo ""

# Function to cleanup background processes
cleanup() {
    echo ""
    echo "🛑 Shutting down services..."
    kill $FRONTEND_PID 2>/dev/null
    echo "✅ Services stopped."
    exit 0
}

# Trap Ctrl+C to cleanup
trap cleanup SIGINT

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
    exit 1
fi

echo ""
echo "🔗 Services Running:"
echo "   Frontend: http://localhost:8000"
echo "   Shared JSON: frontend/books.json"
echo ""
echo "📖 To interact with backend:"
echo "   cd backend && ./library_hashmap"
echo ""
echo "Press Ctrl+C to stop all services"
echo "================================"

# Keep script running and monitor services
while true; do
    if ! kill -0 $FRONTEND_PID 2>/dev/null; then
        echo "❌ Frontend server stopped unexpectedly"
        break
    fi
    sleep 5
done
