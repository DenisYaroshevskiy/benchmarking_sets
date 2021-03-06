import argparse
import json
import plotly
import re


def generateMapOfStyles():
    styles = {}

    styles['Boost'] = dict(
        mode='lines',
        name='Boost',
        line=dict(width=3, dash='dot', color='rgb(000, 100, 255)')
    )

    styles['Srt'] = dict(
        mode='lines',
        name='Srt',
        line=dict(width=3, dash='dash', color='rgb(100, 100, 0)')
    )

    styles['AbslHash'] = dict(
        mode='lines',
        name='AbslHash',
        line=dict(width=3, dash='solid', color='rgb(000, 100, 100)')
    )

    styles['benchmark_merge<merge_v2>'] = dict(
        mode='lines',
        name='boundary_checks',
        line=dict(width=3, dash='dot', color='rgb(100, 000, 100)')
    )

    styles['benchmark_merge<libstd_merge>'] = dict(
        mode='lines',
        name='libstd_merge',
        line=dict(width=3, dash='dashdot', color='rgb(200, 200, 000)')
    )

    styles['benchmark_merge<merge_v3>'] = dict(
        mode='lines',
        name='merge_goto',
        line=dict(width=3, dash='solid', color='rgb(000, 153, 076)')
    )

    styles['benchmark_merge<merge_v4>'] = dict(
        mode='lines',
        name='compiler',
        line=dict(width=3, dash='solid', color='rgb(153, 000, 076)')
    )

    styles['benchmark_merge<merge_v5>'] = dict(
        mode='lines',
        name='unrolled',
        line=dict(width=3, dash='dashdot', color='rgb(200, 200, 000)')
    )

    styles['benchmark_merge<merge_v6>'] = dict(
        mode='lines',
        name='upper_bound_fallback',
        line=dict(width=3, dash='dash', color='rgb(100, 100, 0)')
    )

    styles['benchmark_merge<merge_v7>'] = dict(
        mode='lines',
        name='merge_v7',
        line=dict(width=3, dash='dot', color='rgb(150, 000, 100)')
    )

    styles['benchmark_merge<merge_v8>'] = dict(
        mode='lines',
        name='merge_biased',
        line=dict(width=3, dash='solid', color='rgb(0, 0, 0)')
    )

    styles['benchmark_merge<merge_v9>'] = dict(
        mode='lines',
        name='merge_switch',
        line=dict(width=3, dash='solid', color='rgb(0, 0, 0)')
    )

    styles['benchmark_merge<std_copy>'] = dict(
        mode='lines',
        name='copy',
        line=dict(width=3, dash='dot', color='rgb(150, 000, 100)')
    )

    return styles


class parsedBenchmark:
    def __init__(self, name, xs, times):
        self.name = name
        self.xs = xs
        self.times = times


class runner:
    def __init__(self):
        self.passedBenchmarkList = []
        self.benchmarks = []
        self.data = None
        self.layout = None
        self.smallestX = None
        self.biggestX = None

    def parseFromOptions(self):
        parser = argparse.ArgumentParser(
            description="Draws google benchmark's json output as a graph. \
                     Supports : for providing baselines, like run_operation.json:copy_set.json")
        parser.add_argument('results', metavar='results', nargs='+',
                            help='selected benchmarks to display on one chart')
        parser.add_argument('--smallestX', type=int, dest='smallestX', default=0,
                            help='left boundary on x axis')
        parser.add_argument('--biggestX', type=int, dest='biggestX', default=100000000,
                            help='right boundary on y axis')
        options = parser.parse_args()
        self.passedBenchmarkList = options.results
        self.smallestX = options.smallestX
        self.biggestX = options.biggestX

    def loadJson(self, jsonFile):
        loaded = json.load(open(jsonFile))
        name = re.search(r'<(\w+)>', loaded['benchmarks'][0]['name']).group(1)

        xs = []
        times = []
        for measurement in loaded['benchmarks']:
            parts = measurement['name'].split('/')
            x = int(parts[len(parts) - 1])
            if x < self.smallestX or x > self.biggestX:
                continue
            xs.append(x)
            times.append(float(measurement["real_time"]))
        return parsedBenchmark(name, xs, times)

    def loadJsons(self):
        for passedBenchmark in self.passedBenchmarkList:
            splitByColon = passedBenchmark.split(':')
            bench = splitByColon[0]
            baseline = splitByColon[1] if len(splitByColon) > 1 else None

            benchLoaded = self.loadJson(bench)

            if baseline is not None:
                baselineLoaded = self.loadJson(baseline)
                baselineAsDict = {x: time for (x, time) in zip(
                    baselineLoaded.xs, baselineLoaded.times)}
                benchLoaded.times = [time - baselineAsDict[x]
                                     for (x, time) in zip(benchLoaded.xs, benchLoaded.times)]

            self.benchmarks.append(benchLoaded)

    def generateData(self):
        styles = generateMapOfStyles()
        traces = []
        for benchmark in self.benchmarks:
            traces.append(plotly.graph_objs.Scatter(
                x=benchmark.xs,
                y=benchmark.times,
                **styles[benchmark.name]))
        self.data = traces

    def generateLayout(self):
        layout = {}
        layout['yaxis'] = dict(title='ns')

        self.layout = layout

    def draw(self):
        plotly.offline.plot(
            dict(data=self.data, layout=self.layout), auto_open=True)


if __name__ == "__main__":
    r = runner()
    r.parseFromOptions()
    r.loadJsons()
    r.generateData()
    r.generateLayout()
    r.draw()
