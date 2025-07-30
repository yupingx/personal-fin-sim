# Personal Long-term Finance Simulator

## Motivation
Years ago, when I first started learning about personal financial planning, I realized that even when we have enough math skills to manage the basics of personal finance, we can quickly lose an _intuitive_ sense of how probabilty and the power of exponentials play once the number is larger than a single digit. Just ask yourself and your intelligent friends the likelyhoood of having at least two people share the same birthday in a small office of 50, or how you expect a modest investment to grow over 30 years at a rate of 0.7 vs 0.75. I learned to calculate it diligently, then ask myself if I'm surprised.

While major banks all have simple calculators, they often don't provide transparency of the assumptions under their long-term projection.

These, plus a desire to have my data live locally and the interest to play with hypothetical recessions, motivated me to write this program. 

## About
This is a lightweight personal project designed for running long-term financial projections with a focus on keeping all data local and all underlying assumptions known.

This program is **not** intended to provide financial advice. It is built purely for personal experimentation.

## Key Features
- **Structured User Input**  
  User data such as investment account values, income, contributions, and inflation assumptions are provided via a structured `.ini` file. See [`data/demo_profile.ini`](data/demo_profile.ini) for a template. You can create multiple files to model different scenarios.

- **Recession-Aware Simulation**  
  The simulator includes a randomized recession model based on assumptions defined at compile time (e.g., recession frequency, impact, and recovery duration). See [`include/modelRecession.h`](include/modelRecession.h) for details. Random values are currently drawn from uniform distributions.

- **Fund Longevity Statistics**  
  The simulator estimates how long your retirement funds may last. When using the randomized recession model, it runs multiple iterations to build a probability distribution and summarizes outcomes in binned ranges. Two additional models (constant growth and a predefined "year-0 recession scenario") are included for comparison.


## Limitations

This simulator assumes your investment growth rates are strongly correlated with the long-term performance of the U.S. stock market, but would not exceed its average return (i.e., it does not attempt to model strategies that consistently "beat the market").

If your specified growth rate is lower than the market average, the simulator assumes it reflects a more balanced portfolio allocation with reduced volatility, however that effect is not explicitly modeled.

These simplifications allow the simulation to maintain a rough proportional relationship between your expected rate of return and the overall market return in both growth and loss scenarios while keeping the model lightweight and easy to interpret.

## Usage
### 1. Build Instructions

This project was developed and tested on **macOS Sonoma 14.5** (native) and **Ubuntu 24.04.02** (virtual).

#### macOS

If you're using macOS, you can run the pre-built executable directly:

```bash
git clone https://github.com/yupingx/personal-fin-sim
cd personal-fin-sim
./build/pfsim --user demo
```
#### Linux

```bash
git clone https://github.com/yupingx/personal-fin-sim
cd personal-fin-sim
cmake -S . -B build
cmake --build build
./build/pfsim --user demo
```
### 2. Creating Your Own Profile
To use your own data:

1. Copy `data/demo_profile.ini` and rename it, e.g., `Leia_profile.ini`

2. Edit the file to reflect your current finances and assumptions

3. Run the simulator:

```bash
./build/pfsim --user Leia
```

## Future Feature Expansion Ideas
- **Adaptive Cash Reserve** - Add a realistic cash reserve logic.
- **Tax Estimation** - Add tax impacts to improve withdrawal modeling.
- **Variable Inflation** – Introduce inflation dynamics, possibly correlated with market behavior.
- **Event-based Expense Changes** - Add the flexibility to include variable expenses that occur in real life.
- ... and more.
