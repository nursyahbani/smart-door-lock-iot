<?php
// Connect to the database
$conn = new mysqli('localhost', 'root', '', 'smart_door_lock');

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $rfid = $_POST['rfid'];
    $email = $_POST['email'];
    $password = password_hash($_POST['password'], PASSWORD_BCRYPT);

    $stmt = $conn->prepare("INSERT INTO users (rfid, email, password) VALUES (?, ?, ?)");
    $stmt->bind_param("sss", $rfid, $email, $password);

    if ($stmt->execute()) {
        echo "Registration successful! <a href='login.html'>Login here</a>";
    } else {
        echo "Error: " . $conn->error;
    }

    $stmt->close();
}
$conn->close();
?>
