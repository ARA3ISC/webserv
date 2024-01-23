<?php
// Retrieve environment variables
$requestMethod = getenv('REQUEST_METHOD');
$queryString = getenv('QUERY_STRING');

// Print HTTP headers
header('Content-type: text/html');

// Include additional headers based on the environment variables
if ($requestMethod) {
    header('Request-Method: ' . $requestMethod);
}

if ($queryString) {
    header('Query-String: ' . $queryString);
}

// Output the HTML content
echo '<html><head><title>CGI Test</title></head><body>';
echo '<h1>Hello, CGI!</h1>';
echo '<p>Request Method: ' . ($requestMethod ? $requestMethod : 'Not available') . '</p>';
echo '<p>Query String: ' . ($queryString ? $queryString : 'Not available') . '</p>';
echo '</body></html>';
?>
