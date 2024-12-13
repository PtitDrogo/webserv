#!/usr/bin/python3

import os
import sys

def handle_post_request():
    # Get the content length from environment variable
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    except ValueError:
        # If content length is not a valid integer
        sys.stderr.write("Error: Invalid content length\n")
        return

    # Read exactly the number of bytes specified by content length
    if content_length > 0:
        # Read from standard input
        post_data = sys.stdin.read(content_length)
        
        # Print CGI header
        print("Content-Type: text/plain\n")
        
        # Echo back the received data
        print("Received POST data:")
        print(post_data)
    else:
        # Print CGI header for no content
        print("Content-Type: text/plain\n")
        print("No data received")

if __name__ == "__main__":
    handle_post_request()