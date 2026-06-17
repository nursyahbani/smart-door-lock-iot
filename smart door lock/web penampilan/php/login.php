<?php
// Connect to the database
$conn = new mysqli('localhost', 'root', '', 'smart_door_lock');

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $username = $_POST['username'];
    $password = $_POST['password'];

    $stmt = $conn->prepare("SELECT password FROM users WHERE username = ?");
    $stmt->bind_param("s", $username);
    $stmt->execute();
    $stmt->bind_result($hash);

    if ($stmt->fetch() && password_verify($password, $hash)) {
        header('Location: interface.html');
    } else {
        echo "Invalid credentials. <a href='login.html'>Try again</a>";
    }

    $stmt->close();
}
$conn->close();
?>
