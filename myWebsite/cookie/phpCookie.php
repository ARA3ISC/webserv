<?php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Process form submission
    if (isset($_POST['cookie_content'])) {
        // Set cookie
        $cookie_name = "example_cookie";
        $cookie_value = $_POST['cookie_content'];
        $expiration_time = time() + (3600); // 3600 seconds = 1 hour
        setcookie($cookie_name, $cookie_value, $expiration_time, "/");

        // Redirect back to the same page to see the cookie in action
        header("Location: ".$_SERVER['PHP_SELF']);
        exit();
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Set Cookie</title>
</head>
<body>
    <h2>Set Cookie</h2>
    <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
        Cookie Content: <input type="text" name="cookie_content"><br><br>
        <input type="submit" value="Set Cookie">
    </form>
    <?php
    // Display cookie value if set
    if (isset($_COOKIE['example_cookie'])) {
        echo "Cookie 'example_cookie' is set with value: " . $_COOKIE['example_cookie'];
    }
    ?>
</body>
</html>
