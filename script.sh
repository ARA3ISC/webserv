#!/bin/bash

for ((i=1; i<=200; i++)); do
  # curl http://127.0.0.1:9999  # replace with your actual URL
  # curl -X GET -H "Content-Type: image/jpeg" --data-binary "@./myWebsite/contact/output1706560967.jpeg" http://127.0.0.1:9999/
  # curl -X GET -H "Content-Type: image/jpeg" http://127.0.0.1:9999/contact/output1706560967.jpeg --output image$i.jpeg
  echo "Request $i sent"
done
