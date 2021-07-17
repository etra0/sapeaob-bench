import argparse
import io
import matplotlib.pyplot as plt
import pandas as pd
import subprocess
import seaborn as sns

def generate_data(program_name: str) -> pd.DataFrame:
    print("Running benchmark...")
    execution = subprocess.run([program_name, "--benchmark_format=csv"], capture_output=True)
    print("Processing data...")
    data = io.StringIO(execution.stdout.decode('utf-8'))
    df = pd.read_csv(data, sep=',')

    parsed_df = (
     pd.concat([df, df.name.str.extract(r"BM_(?P<library>[A-Za-z]+)_(?P<task>.*)")], axis=1)
    )

    return parsed_df

def plot(data: pd.DataFrame, name: str) -> None:
    plot = sns.catplot(x="task", y="cpu_time", hue="library", kind="bar", data=data)
    # [ax.xaxis.set_tick_params(rotation=90) for ax in plot.axes.flatten()]
    [ax.set_ylabel("cpu time (ns)") for ax in plot.axes.flatten()]
    plot.fig.suptitle("Different pattern sizes in a 10MB array. (lower is better)", y=1.01)

    plot.savefig(name, dpi=300)


def main():
    parser = argparse.ArgumentParser(description="Generates the plot of the data.")
    parser.add_argument("binary", help="Path of the binary which will be executed")
    parser.add_argument("--output", help="Name of the plot file (default is output.png)", default="output.png")
    args = parser.parse_args()
    data = generate_data(args.binary)
    plot(data, args.output)


if __name__ == "__main__":
    main()
