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
    // Check if form data was submitted

    if ($_SERVER["REQUEST_METHOD"] == "GET") {
        // Retrieve and display submitted data
        $name = $_GET["name"];
        $time = $_GET["time"];
        
        // Display the submitted data
        echo "<p><strong>Name:</strong> $name</p>";
        echo "<p><strong>time:</strong> $time</p>";
    } else {
        // If no form data was submitted, display an error message
        echo "<p>No contact information available.</p>";
    }
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Retrieve and display submitted data
        $name = $_POST["name"];
        $time = $_POST["time"];
        
        // Display the submitted data
        echo "<p><strong>Name:</strong> $name</p>";
        echo "<p><strong>time:</strong> $time</p>";
    } else {
        // If no form data was submitted, display an error message
        echo "<p>No contact information available.</p>";
    }
    ?>
        ?>
    <a href="index.html">Home</a>
    <a href="about.html">About</a>
    <a href="contact.html">Contact</a>
</body>
</html>
