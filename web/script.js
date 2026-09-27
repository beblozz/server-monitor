function loadMetrics()
{
    fetch("/api/metrics")
        .then(response => response.json())
        .then(data =>
        {
            updateMetrics(data);

            updateNetwork(data.network);

            updateProcesses(data.processes);
        })
        .catch(error =>
        {
            console.log(
                "Metrics error:",
                error
            );
        });
}


function loadStatus()
{
    fetch("/api/status")
        .then(response => response.json())
        .then(data =>
        {
            updateStatus(
                "server-status",
                data.status === "online",
                "ONLINE",
                "OFFLINE"
            );


            updateStatus(
                "database-status",
                data.database === "connected",
                "CONNECTED",
                "DISCONNECTED"
            );


            updateStatus(
                "cpu-status",
                data.cpu === "ok",
                "OK",
                "WARNING"
            );


            updateStatus(
                "memory-status",
                data.memory === "ok",
                "OK",
                "WARNING"
            );


            updateStatus(
                "disk-status",
                data.disk === "ok",
                "OK",
                "WARNING"
            );
        })
        .catch(error =>
        {
            console.log(
                "Status error:",
                error
            );

            updateStatus(
                "server-status",
                false,
                "ONLINE",
                "OFFLINE"
            );

            updateStatus(
                "database-status",
                false,
                "CONNECTED",
                "DISCONNECTED"
            );
        });
}


function updateStatus(
    elementId,
    isOk,
    okText,
    errorText
)
{
    const element =
        document.getElementById(elementId);


    if (!element)
    {
        return;
    }


    if (isOk)
    {
        element.textContent =
            okText;

        element.className =
            "status-ok";
    }
    else
    {
        element.textContent =
            errorText;

        element.className =
            "status-warning";
    }
}


function updateMetrics(data)
{
    const cpu =
        Number(data.cpu);

    const memory =
        Number(data.memory);

    const disk =
        Number(data.disk);


    document.getElementById(
        "cpu-value"
    ).textContent =
        cpu.toFixed(2) + "%";


    document.getElementById(
        "memory-value"
    ).textContent =
        memory.toFixed(2) + "%";


    document.getElementById(
        "disk-value"
    ).textContent =
        disk.toFixed(2) + "%";


    document.getElementById(
        "cpu-bar"
    ).style.width =
        Math.min(cpu, 100) + "%";


    document.getElementById(
        "memory-bar"
    ).style.width =
        Math.min(memory, 100) + "%";


    document.getElementById(
        "disk-bar"
    ).style.width =
        Math.min(disk, 100) + "%";
}


function updateNetwork(network)
{
    const table =
        document.getElementById(
            "network-table"
        );


    table.innerHTML = "";


    network.forEach(item =>
    {
        const row =
            document.createElement("tr");


        row.innerHTML = `
            <td>${item.name}</td>
            <td>${formatBytes(item.received)}</td>
            <td>${formatBytes(item.transmitted)}</td>
        `;


        table.appendChild(row);
    });
}


function updateProcesses(processes)
{
    const table =
        document.getElementById(
            "process-table"
        );


    table.innerHTML = "";


    processes.forEach(process =>
    {
        const row =
            document.createElement("tr");


        row.innerHTML = `
            <td>${process.pid}</td>
            <td>${process.name}</td>
            <td>${Number(process.cpu).toFixed(2)}%</td>
            <td>${process.memory} KB</td>
        `;


        table.appendChild(row);
    });
}


function loadHistory()
{
    fetch("/api/history")
        .then(response => response.json())
        .then(data =>
        {
            updateHistory(data);

            drawChart(data);
        })
        .catch(error =>
        {
            console.log(
                "History error:",
                error
            );
        });
}


function updateHistory(history)
{
    const table =
        document.getElementById(
            "history-table"
        );


    table.innerHTML = "";


    history.forEach(item =>
    {
        const row =
            document.createElement("tr");


        row.innerHTML = `
            <td>${item.created_at}</td>
            <td>${Number(item.cpu).toFixed(2)}%</td>
            <td>${Number(item.memory).toFixed(2)}%</td>
            <td>${Number(item.disk).toFixed(2)}%</td>
        `;


        table.appendChild(row);
    });
}


function drawChart(history)
{
    const canvas =
        document.getElementById(
            "history-chart"
        );


    const context =
        canvas.getContext("2d");


    context.clearRect(
        0,
        0,
        canvas.width,
        canvas.height
    );


    if (history.length === 0)
    {
        return;
    }


    const width =
        canvas.width;

    const height =
        canvas.height;


    const padding = 30;


    /*
        Draw horizontal lines
    */

    context.beginPath();


    for (
        let i = 0;
        i <= 4;
        i++
    )
    {
        const y =
            padding +
            (
                (height - padding * 2)
                / 4
            ) * i;


        context.moveTo(
            padding,
            y
        );


        context.lineTo(
            width - padding,
            y
        );
    }


    context.strokeStyle =
        "#333";


    context.stroke();


    /*
        Draw CPU line
    */

    drawLine(
        context,
        history,
        "cpu",
        width,
        height,
        padding
    );


    /*
        Draw memory line
    */

    drawLine(
        context,
        history,
        "memory",
        width,
        height,
        padding
    );


    /*
        Draw disk line
    */

    drawLine(
        context,
        history,
        "disk",
        width,
        height,
        padding
    );
}


function drawLine(
    context,
    history,
    property,
    width,
    height,
    padding
)
{
    context.beginPath();


    history =
        [...history].reverse();


    history.forEach(
        (item, index) =>
        {
            const x =
                padding +
                (
                    index /
                    Math.max(
                        history.length - 1,
                        1
                    )
                ) *
                (
                    width -
                    padding * 2
                );


            const value =
                Number(
                    item[property]
                );


            const y =
                height -
                padding -
                (
                    Math.min(
                        value,
                        100
                    ) / 100
                ) *
                (
                    height -
                    padding * 2
                );


            if (index === 0)
            {
                context.moveTo(
                    x,
                    y
                );
            }
            else
            {
                context.lineTo(
                    x,
                    y
                );
            }
        }
    );


    context.strokeStyle =
        "#ffffff";


    context.stroke();
}


function formatBytes(bytes)
{
    const value =
        Number(bytes);


    if (value < 1024)
    {
        return value + " B";
    }


    if (value < 1024 * 1024)
    {
        return (
            value / 1024
        ).toFixed(2) + " KB";
    }


    if (value < 1024 * 1024 * 1024)
    {
        return (
            value /
            (1024 * 1024)
        ).toFixed(2) + " MB";
    }


    return (
        value /
        (1024 * 1024 * 1024)
    ).toFixed(2) + " GB";
}


/*
    Initial loading
*/

loadMetrics();

loadStatus();

loadHistory();


/*
    Automatic updates
*/

setInterval(
    loadMetrics,
    3000
);


setInterval(
    loadStatus,
    3000
);


setInterval(
    loadHistory,
    5000
);