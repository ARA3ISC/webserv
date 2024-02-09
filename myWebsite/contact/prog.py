#!/usr/bin/env python

import os

# Retrieve environment variables
request_method = os.environ.get('REQUEST_METHOD', '')
query_string = os.environ.get('QUERY_STRING', '')

# Print HTTP headers
print()

# Include additional headers based on the environment variables
if request_method:
    print("Request-Method: {}".format(request_method))

if query_string:
    print("Query-String: {}".format(query_string))

# Output the HTML content
print("<html><head><title>CGI Test</title></head><body>")
print("<h1>Hello from CGI!</h1>")
print("<h2>this is a .py script test</h2>")
print("<p>Request Method: {}</p>".format(request_method or 'Not available'))
print("<p>Query String: {}</p>".format(query_string or 'Not available'))
print("</body></html>")
