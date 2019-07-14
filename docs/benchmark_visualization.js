'use strict';

function getName(benchmarLongkName) {
    return benchmarLongkName.split('<')[1].split('>')[0];
}

function transformGoogleBenchmarkOutput(loadedJson) {
    let result = {
        name: '',
        xs: [],
        times: []
    };
    result.name = getName(loadedJson.benchmarks[0].name);

    loadedJson.benchmarks.forEach(measurement => {
        let parts = measurement.name.split('/');
        result.xs.push(parts[parts.length - 1]);
        result.times.push(measurement.real_time);
    });

    return result;
}

function loadGoogleBenchmarkResults(file) {
    file = 'computed_jsons/' + file + '.json';
    return fetch(file).then(
        (resp) => {
            return resp.json();
        }
    ).then(
        (loadedJson) => {
            return transformGoogleBenchmarkOutput(loadedJson);
        }
    );
}

function zipArrays(arr1, arr2) {
    return arr1.map((x, i) => [x, arr2[i]]);
}

function subtractBaseline(main, baseline) {
    const baselineMap = new Map(zipArrays(baseline.xs, baseline.times));
    return {
        name: main.name,
        xs: main.xs,
        times: main.times.map((time, i) => {
            return time - baselineMap.get(main.xs[i]);
        })
    };
}

function loadBenchmark(benchmark) {
    const split = benchmark.split(':');
    console.log(split);
    const main = loadGoogleBenchmarkResults(split[0]);
    if (split.length === 1) {
        return main;
    }
    const baseline = loadGoogleBenchmarkResults(split[1]);
    return Promise.all([main, baseline]).then((ps) => subtractBaseline(ps[0], ps[1]));
}

function transformBenchmarksToDataTable(benchmarks) {
    const titles = [''].concat(benchmarks.map((b) => b.name));
    const rows = benchmarks[0].xs.map(
        (x, i) => {
            return [x].concat(benchmarks.map((b) => b.times[i]));
        }
    );

    const asArray = [titles].concat(rows);
    return google.visualization.arrayToDataTable(asArray);
}

function drawBenchmarksChart(id, data) {
    let options = {
        curveType: 'function',
        legend: { position: 'bottom' }
    };
    console.log(data);
    let chart = google.visualization.LineChart(document.getElementById(id));
    chart.draw(data, options);
}

function visualizeBenchmarks(id, ...benchmarks) {
    google.charts.load('current', { packages: ['corechart'] });
    google.charts.setOnLoadCallback(drawChart);

    const loadedBenchmarks = Promise.all(benchmarks.map(loadBenchmark));

    function drawChart() {
        loadedBenchmarks.then(
            (benchmarks) => {
                const data = transformBenchmarksToDataTable(benchmarks);
                console.log(data);
                drawBenchmarksChart(id, data);
            }
        );
    }
}