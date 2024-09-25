// Placeholder functions for scanning and connecting WiFi
window.onload = fetchNetworkInfo();
setTimeout(scanNetworks, 1000);
var ip;
function fetchNetworkInfo() {
    fetch('/networkinfo')
        .then(response => response.json())  // Wait for the response to resolve to JSON
        .then(data => {  // Use the JSON data after it is resolved
            document.getElementById('ssid_connected').innerText = data.ssid;
            document.getElementById('ip').innerText = data.ip;
            document.getElementById('mac').innerText = data.mac;
            ip = data.ip;
            console.log(data);
            console.log(ip);
        })
        .catch(error => console.error('Error fetching network info:', error));  // Catch any errors
}
function scanNetworks() {
    console.log(ip);
    // ip = document.getElementById('ip').textContent.trim();
    var url = `http://${ip}/scan`;
    console.log(url);
    fetch(url)
        .then(response => response.json())
        .then(networks => {
            const networksDiv = document.getElementById('networks');
            networksDiv.innerHTML = networks.length ? '<label for="ssid">Select Network:</label>' : 'No networks found.';
            const select = document.createElement('select');
            select.id = 'ssid';
            select.name = 'ssid';
            select.onchange = function () {
                const selectedSSID = select.value;
                document.getElementById('ssidInput').value = selectedSSID; // Update the SSID input field
            };
            networks.forEach(network => {
                const option = document.createElement('option');
                option.value = network.ssid;
                option.text = network.ssid + ' (RSSI: ' + network.rssi + ')';
                select.appendChild(option);
            });
            networksDiv.appendChild(select);
        })
        .catch(error => {
            console.error('Error fetching scan results:', error);
        });
}
function submitNetwork() {
    var ssid = document.getElementById('ssidInput').value.trim();
    var password = document.getElementById('password').value.trim();
    console.log(ssid, password);
    var x= confirm(`Connecting to ${ssid} with Password: ${password}! ESP will Reset.`);
    if (x) {
        // If user clicks "OK", submit the form
        document.getElementById('myForm').submit();
    } else {
        // If user clicks "Cancel", do nothing
        console.log("Form submission cancelled.");
    }
}