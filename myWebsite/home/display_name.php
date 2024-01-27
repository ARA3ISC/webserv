<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Display Full Name</title>
</head>
<body>

    <h2>Full Name Display</h2>

    <?php
        // Retrieve the full name from the query string
        $fullName = isset($_GET['fullName']) ? $_GET['fullName'] : '';

        // Display the full name
        echo "<p>Your Full Name: $fullName</p>";
    ?>

</body>
</html>
