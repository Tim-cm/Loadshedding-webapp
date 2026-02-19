// Handles the responsiveness of the web page when you 
// press the simulate and add button

let numberOfLoads = 0;

// Adds a load row for the user to enter values
function addLoad() {
    const div = document.createElement("div");
    div.className = "load-row";

    div.innerHTML = `
        <input placeholder="Load name" id="name${numberOfLoads}">
        <input placeholder="Power (MW)" type="number" id="power${numberOfLoads}">
        <input placeholder="Priority" type="number" id="priority${numberOfLoads}">
    `;

    document.getElementById("loads").appendChild(div);
    numberOfLoads++;
}

function simulate() {
    const capacity = document.getElementById("capacity").value;
    const data = new URLSearchParams();

    data.append("capacity", capacity);
    data.append("count", numberOfLoads);

    for (let i = 0; i < numberOfLoads; i++) {
        data.append("name" + i, document.getElementById("name" + i).value);
        data.append("power" + i, document.getElementById("power" + i).value);
        data.append("priority" + i, document.getElementById("priority" + i).value);
    }

    fetch("http://localhost:8080/shed", {method: "POST", body: data})
    .then(serverResponse => serverResponse.json())
    .then(jsonFile => {
        document.getElementById("result").textContent = "Served Loads:\n" + jsonFile.served.join(", ") + 
        "\nShed load:\n" + jsonFile.shed.join(", ") + "\n=======================" +
        "\nTotal Used Power: " + jsonFile.used + " MW"; });
}
