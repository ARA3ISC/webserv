<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Display Name</title>
</head>
<body>
    <?php
    $pathInfo = $_SERVER['PATH_INFO'];
    $contentType = $_SERVER['CONTENT_TYPE'];
    $contentLength = $_SERVER['CONTENT_LENGTH'];
    $scriptName = $_SERVER['SCRIPT_NAME'];
    $serverProtocol = $_SERVER['SERVER_PROTOCOL'];
    
    $QUERY_STRING = $_SERVER['QUERY_STRING'];
    echo "pathInfo : ";
    echo $pathInfo;
    echo "<br>";

    echo "contentType : ";
    echo $contentType;
    echo "<br>";

    echo "contentLength : ";
    echo $contentLength;
    echo "<br>";

    echo "scriptName : ";
    echo $scriptName;
    echo "<br>";

    echo "serverProtocol : ";
    echo $serverProtocol;
    echo "<br>";
    
    echo $_POST;
    // Check if the request method is POST
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        $postData = file_get_contents('php://input');

        // Print the request body
        echo "body : " ;echo $postData; echo "<br>";
        // Check if the 'name' field exists in the POST data
        print_r($_POST);
        if(isset($_POST['name'])) {
            // Retrieve the submitted name
            $name = $_POST['name'];
            
            // Display the submitted name
            echo "<p>Hello, $name!</p>";
        } else {
            // Handle case when 'name' field is not found
            echo "<p>name not found</p>";
        }
    } 
    if ($_SERVER["REQUEST_METHOD"] == "GET"){
        echo "QUERY_STRING : "; echo $QUERY_STRING . "<br>";
        if(isset($_GET['name'])) {
            // Retrieve the submitted name
            $name = $_GET['name'];
    
            // Display the submitted name
            echo "<p>Hello, $name!</p>";
        } else {
            // Handle case when 'name' field is not found
            echo "<p>name not found :( </p>";
        }
    }  
    else {
        // Handle case when form is not submitted
        echo "<p>No data submitted.</p>";
    }
    ?>
</body>
</html>
