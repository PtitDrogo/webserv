#! /usr/bin/python3
import os

def main():
    print("Hello basicparam.py is here")
    # Retrieve the "name" and "age" parameters from the environment
    name = os.environ.get('name')
    age = os.environ.get('age')

    # Print the values
    print("<html><body>")
    print("<h2>CGI Script Parameters</h2>")
    print("<ul>")
    print(f"<li>name: {name}</li>")
    print(f"<li>age: {age}</li>")
    print("</ul>")
    print("</body></html>")

if __name__ == "__main__":
    main()