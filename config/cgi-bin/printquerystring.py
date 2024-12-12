#!/usr/bin/python3

import os
import urllib.parse

def main():
    # Get the QUERY_STRING environment variable
    query_string = os.environ.get('QUERY_STRING', '')

    # Parse the query string into a dictionary
    params = urllib.parse.parse_qs(query_string)

    # Print the parameters
    print("Received Parameters:")
    for key, values in params.items():
        print(f"{key}: {', '.join(values)}")

if __name__ == "__main__":
    main()
