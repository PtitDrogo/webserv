#!/usr/bin/env python3

import os
import cgi

# Print the necessary CGI headers
print("Content-Type: text/html")
# print()

# Get the query parameters
query_string = os.environ.get("QUERY_STRING", "")

# Parse the query parameters
params = cgi.parse_qs(query_string)

# Print the parameters
print("<html><body>")
print("<h2>CGI Script Parameters</h2>")
print("<ul>")
for key, value in params.items():
    print(f"<li>{key}: {', '.join(value)}</li>")
print("</ul>")
print("</body></html>")
