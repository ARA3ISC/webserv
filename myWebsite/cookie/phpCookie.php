<!DOCTYPE html>
<html>
<head>
    <title>Set Cookie on Form Submit</title>
</head>
<body>
    <h2>Submit Your Username</h2>
    <form method="post" action="/cookie/phpCookie.php">
        <label for="username">Username:</label><br>
        <input type="text" id="username" name="username"><br><br>
        <input type="submit" value="Submit">
    </form>

    <?php
    // Check if form is submitted
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Check if username is set and not empty
        if (isset($_POST["username"]) && !empty(trim($_POST["username"]))) {
            // Set a cookie with the username
            $username = htmlspecialchars($_POST["username"]);
            $cookie_name = "username_cookie";
            $cookie_value = $username;
            // Cookie expires in 1 day (86400 seconds)
            $expiration = time() + (86400 * 1); 
            setcookie($cookie_name, $cookie_value, $expiration, "/");
            echo "<p>Cookie '" . $cookie_name . "' is set with value: '" . $cookie_value . "'</p>";
        } else {
            echo "<p>Please enter a username.</p>";
        }
    }
    ?>

</body>
</html>
