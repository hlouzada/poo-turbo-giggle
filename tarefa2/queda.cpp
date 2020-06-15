#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

//-----------------------------------------------------------------------------
//
// Representing measurements with errors.
//
//

// Class to represent an experimental measurement value.
class Measurement {
  public:
    float value; // Measured value
    float error; // Associated error
  
    //-----------------------------------------------------------------------------
    // Arithmetic operations on measurements.
    Measurement &operator+=(Measurement const &);
    Measurement &operator-=(Measurement const &);
    Measurement &operator*=(Measurement const &);
    Measurement &operator/=(Measurement const &);
  
    // Some two measurements. Evaluate error.
    friend Measurement operator+(Measurement const &a, Measurement const &b);
  
    // Subtract two measurements. Evaluate error.
    friend Measurement operator-(Measurement const &a, Measurement const &b);
  
    // Multiply two measurements. Evaluate error.
    friend Measurement operator*(Measurement const &a, Measurement const &b);
  
    // Multiply a constant with a measurement. Evaluate error.
    friend Measurement operator*(float a, Measurement const &b);
  
    // Divide two measurements. Evaluate error.
    friend Measurement operator/(Measurement const &a, Measurement const &b);
  
    // Divide a measurement by a constant. Evaluate error.
    friend Measurement operator/(Measurement const &a, float b);
};

//-----------------------------------------------------------------------------
// Type and class to represent the time and positions of the particle. With errors.
//
struct ParticlePosition {
  Measurement time;   // Time
  Measurement height; // Associated height
};

class Positions {
  public:
    std::vector<ParticlePosition> data;

    // Reads data from filename.
    std::vector<ParticlePosition> read_data(std::string filename);
    // Class contructor
    Positions(std::string _filename) {
     data = read_data(_filename);
    };
};

//-----------------------------------------------------------------------------
//
// Auxiliary classes and functions for the main function.
//

// Some useful type synonyms.
//using Positions = std::vector<ParticlePosition>;

class Compute : private Positions {
  private:
    // Computes the value of g given the time and height data.
    Measurement calculate_g();
    // Compute velocities in each instant given the data and
    // already evaluated g.
    std::vector<Measurement> calculate_velocities(Measurement g);
  public:
    Measurement g;
    std::vector<Measurement> velocities;
    // Class contructor
    Compute(std::string _filename) : Positions(_filename) {
      g = calculate_g();
      velocities = calculate_velocities(g);
    };
};

// Tells how to execute the code.
void usage(std::string exename);

//-----------------------------------------------------------------------------
//
// main
//
// Reads data on the trajectory of an object in free fall.
// The name of the file is read from the command line (only argument).
// Data is expected to consist in lines with 4 floating point values each:
// time time-error height height-error
//
// The lines are from the smallest time to the largest time values.
//
// Evaluates and prints to standard output the gravitational acceleartion
// and the velocities at each instant (in order).
//
int main(int argc, char const *argv[]) {
  // We need an argument with the name of the data file.
  if (argc != 2) {
    usage(argv[0]);
    std::exit(1);
  }

  auto data = Compute(argv[1]);
  
  auto g = data.g;
  auto velocities = data.velocities;

  std::cout << "Evaluated values follow.\n\n";
  std::cout << "Gravitational acceleration: " << g.value << " +- " << g.error
            << std::endl;
  std::cout << "Velocities:\n";
  for (size_t i = 0; i < velocities.size(); ++i) {
    std::cout << velocities[i].value << " +- " << velocities[i].error
              << std::endl;
  }

  return 0;
}

//-----------------------------------------------------------------------------
//
// Implementations of the functions auxiliary to main.
//

// Tells how to execute the code.
void usage(std::string exename) {
  std::cerr << "Usage: " << exename << " <data file name>\n";
}

// Reads data from filename.
// The data are in the format:
//
// <time> <time error> <height> <height error>.
//
// All are floating point numbers.
std::vector<ParticlePosition> Positions::read_data(std::string filename) {

  std::ifstream datafile(filename);
  if (!datafile.good()) {
    std::cerr << "Error reading " << filename << std::endl;
    std::exit(2);
  }

  // Read a position (time+height with errors) value.
  float value, error;
  // Try to read until the end of the file.
  while (datafile >> value) {
    // If we find a value, there must be 3 more values.

    datafile >> error;
    if (datafile.fail()) {
      std::cerr << "Error reading data from " << filename << std::endl;
      std::exit(3);
    }

    Measurement time{value, error};

    datafile >> value;
    datafile >> error;
    if (datafile.fail()) {
      std::cerr << "Error reading data from " << filename << std::endl;
      std::exit(3);
    }

    Measurement height{value, error};

    data.push_back({time, height});
  }

  return data;
}

// Computes the value of g given the time and height data.
Measurement Compute::calculate_g() {
  // Uses the first, second and last positions and corresponding times,
  // and compute
  //
  // 2 [(hn-h1)t0-(hn-h0)t1+(h1-h0)tn] / [(t1-t0)(tn-t1)(tn-t0)]
  //
  // (where t is time, h is height and 0, 1, n indicate first, second and last
  // points.)
  auto num_points = data.size();
  auto t0 = data[0].time;
  auto t1 = data[1].time;
  auto tn = data[num_points - 1].time;
  auto h0 = data[0].height;
  auto h1 = data[1].height;
  auto hn = data[num_points - 1].height;

  auto delta_h_10 = h1 - h0;
  auto delta_h_n0 = hn - h0;
  auto delta_h_n1 = hn - h1;
  auto delta_t_10 = t1 - t0;
  auto delta_t_n0 = tn - t0;
  auto delta_t_n1 = tn - t1;
  auto factor1 = delta_h_n1 * t0;
  auto factor2 = delta_h_n0 * t1;
  auto factor3 = delta_h_10 * tn;
  auto numerator = (factor1 - factor2) + factor3;
  auto denominator = (delta_t_10 * delta_t_n1) * delta_t_n0;
  return 2.0f * (numerator / denominator);
}

// Compute velocities in each instant given the data and
// already evaluated g.
std::vector<Measurement> Compute::calculate_velocities(Measurement g) {
  auto const n_data = data.size();
  std::vector<Measurement> velocities(n_data);

  // For each data point (except the last, see below), evaluate the velocity as
  // the starting velocity for a free fall to reach the next point.
  //
  // v = delta_h/delta_t + g*delta_t/2
  //
  for (size_t i = 0; i < n_data - 1; ++i) {
    auto delta_h = data[i + 1].height - data[i].height;
    auto delta_t = data[i + 1].time - data[i].time;
    velocities[i] =
        (delta_h / delta_t) + ((g * delta_t) / 2.0f);
  }

  // The last velocity is evaluated from the one before last and the value of g.
  auto last_delta_t = data[n_data - 1].time - data[n_data - 2].time;
  velocities[n_data - 1] = velocities[n_data - 2] - (g * last_delta_t);

  return velocities;
}

//-----------------------------------------------------------------------------
//
// Implementation of arithmetic operations on measurements with errors.
// The error propagation formulas assume that the error are Gaussian
// and independent (uncorrelated) in the two measurements.
//

inline float square(float x) { return x * x; }

Measurement operator+(Measurement const &a, Measurement const &b) {
  return {a.value + b.value, std::sqrt(square(a.error) + square(b.error))};
}

Measurement operator-(Measurement const &a, Measurement const &b) {
  return {a.value - b.value, std::sqrt(square(a.error) + square(b.error))};
}

Measurement operator*(Measurement const &a, Measurement const &b) {
  auto value = a.value * b.value;
  return {value, std::fabs(value) * std::sqrt(square(a.error / a.value) +
                                              square(b.error / b.value))};
}

Measurement operator*(float a, Measurement const &b) {
  return {a * b.value, std::fabs(a) * b.error};
}

Measurement operator/(Measurement const &a, Measurement const &b) {
  auto value = a.value / b.value;
  return {value, std::fabs(value) * std::sqrt(square(a.error / a.value) +
                                              square(b.error / b.value))};
}

Measurement operator/(Measurement const &a, float b) {
  return {a.value / b, a.error / std::fabs(b)};
}
