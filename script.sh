#!/bin/bash

for ((i=1; i<=10; i++)); do
  # curl http://127.0.0.1:9999  # replace with your actual URL
  # curl -X GET -H "Content-Type: image/jpeg" --data-binary "@./myWebsite/contact/output1706560967.jpeg" http://127.0.0.1:9999/
  # curl -X GET -H "Content-Type: image/jpeg" http://127.0.0.1:9999/contact/output1706560967.jpeg --output image$i.jpeg
  # curl -X POST -H "Content-Type: image/jpeg" --data-binary "@./myWebsite/contact/output1706560989.png" http://127.0.0.1:9999/home/test
  curl -X POST -H "Host: 127.0.0.1" -H "Content-Type: video/mp4" --data-binary "@./myWebsite/contact/videoplayback_2.mp4" http://127.0.0.1:9090/
  echo "Request $i sent"
done
