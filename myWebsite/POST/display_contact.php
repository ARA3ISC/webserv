<!-- display_contact.php -->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Contact Information</title>
</head>
<body>
    <h1>Contact Information</h1>
    <?php

    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Retrieve and display submitted data
        if (isset($_POST["name"]))
            $name = $_POST["name"];
        else
            $name = "";
        if (isset($_POST["time"]))
            $time = $_POST["time"];
        else
            $time = "";

        // Display the submitted data
        echo "<p><strong>Name:</strong> $name</p>";
        echo "<p><strong>time:</strong> $time</p>";
    } else {
        // If no form data was submitted, display an error message
        echo "<p>No contact information available.</p>";
    }
    ?>
    <a href="contact.html">Contact</a>
    <a href="uploadFile.html">Upload File Chunekd</a>
</body>
</html>
