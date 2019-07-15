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
    const main = loadGoogleBenchmarkResults(split[0]);
    if (split.length === 1) {
        return main;
    }
    const baseline = loadGoogleBenchmarkResults(split[1]);
    return Promise.all([main, baseline]).then((ps) => subtractBaseline(ps[0], ps[1]));
}

function divideByX(bench) {
    bench.times = bench.times.map((time, i) => time / bench.xs[i])
    return bench;
}

function transformBenchmarksToDataTable(benchmarks) {
    const titles = [''].concat(benchmarks.map((b) => b.name));
    const rows = benchmarks[0].xs.map(
        (x, i) => {
            return [x].concat(benchmarks.map((b) => b.times[i]));
        }
    );

    return google.visualization.arrayToDataTable([titles].concat(rows));;
}

function drawBenchmarksChart(id, element, data) {
      var options = {
        title: id,
        legend: { position: 'bottom' },
        height: 600,
        width: 800
      };

      let chart = new google.visualization.LineChart(element);
      chart.draw(data, options);
}

function visualizeBenchmarks(id, element, shouldDivideByX, benchmarks) {
    google.charts.load('current', { packages: ['corechart'] });
    google.charts.setOnLoadCallback(drawChart);

    let loaded = benchmarks.map(loadBenchmark);
    if (shouldDivideByX) {
        loaded = loaded.map((b) => b.then(divideByX));
    }

    function drawChart() {
        Promise.all(loaded).then(
            (benchmarks) => {
                const data = transformBenchmarksToDataTable(benchmarks);
                drawBenchmarksChart(id, element, data);
            }
        );
    }
}

function visualizeAllSetsBenchmarks(id, element, shouldDivideByX) {
    const names = ['absl', 'boost', 'srt', 'std', 'std_unordered'].map(
        (container) => id + '_' + container
    );
    visualizeBenchmarks(id, element, shouldDivideByX, names);
}

function visualizeBenchmarksNoStd(id, element, shouldDivideByX) {
    const names = ['absl', 'boost', 'srt'].map(
        (container) => id + '_' + container
    );
    visualizeBenchmarks(id + '_no_std', element, shouldDivideByX, names);
}

function addNewChartDropDown(section) {
    let dropDown = document.createElement("section");
    let dropDownChart = document.createElement("div");
    dropDownChart.className = "benchmarkChart";
    dropDown.appendChild(dropDownChart);
    section.appendChild(dropDown);
    return dropDownChart;
}

function addBencharkCharts(id, shouldDivideByX) {
    let section = document.getElementById(id);
    visualizeAllSetsBenchmarks(id, addNewChartDropDown(section), shouldDivideByX);
    visualizeBenchmarksNoStd(id, addNewChartDropDown(section), shouldDivideByX);
}