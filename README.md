# AME Scanner

## Project Overview

AME Scanner is an autonomous sensory component of the AMAR Engine ecosystem, designed to convert raw Gaussian Splatting (3DGS) radiance fields into structured spatial data. It serves as a critical input layer for the AMAR Engine, providing raw structured information to the AME Admin for subsequent metaclass inference.

## Core Functions

### 1. 3DGS Data Loading
- Supports Gaussian Splatting data in .ply and .splat formats
- Extracts essential geometric properties (position, color, opacity, scale, rotation)
- Converts data into internal Gaussian representation

### 2. Spatial Density Analysis
- Constructs continuous density approximation from Gaussian distributions
- Computes local density at arbitrary points
- Identifies dense regions using density thresholding
- Provides comprehensive density statistics

### 3. Clustering and Region Segmentation
- Segments continuous fields into candidate structural blocks using DBSCAN algorithm
- Outputs anonymous, unclassified spatial clusters
- Supports configurable clustering parameters (epsilon, min points)

### 4. Surface Candidate Extraction
- Identifies potential boundary surfaces based on curvature analysis
- Performs local PCA and normal estimation for each Gaussian
- Computes curvature statistics to distinguish surface regions
- Groups surface candidates into connected regions

### 5. Spatial Relationship Graph Construction
- Generates spatial topology relationships between entities
- Identifies adjacency, containment, and relative position relationships
- Creates a purely geometric relationship graph
- Supports graph visualization in DOT format

## Technical Architecture

AME Scanner is structured in three main components:

### 1. scanner-core
The core algorithmic component that performs:
- 3DGS loading and parsing (via FieldLoader)
- Density field analysis (via DensityAnalyzer)
- Clustering operations (via DBSCAN)
- Surface candidate extraction (via SurfaceExtractor)
- Spatial relationship graph construction (via SpatialGraph)

### 2. scanner-cli
An offline tool that:
- Reads 3DGS files (.ply, .splat)
- Invokes scanner-core functionality
- Outputs SpatialStructurePackage in binary format
- Provides command-line parameters for configuration

### 3. scanner-engine-adapter
An integration component that:
- Converts SpatialStructurePackage to Information Pool format
- Directly injects data into AME Admin

## Output Format

AME Scanner produces a SpatialStructurePackage containing:

- **Scene Bounding Box**: OBB (Oriented Bounding Box) for the entire scene
- **Entities**: List of AmeEntity objects, each containing:
  - Unique ID
  - OBB (Oriented Bounding Box)
  - Mesh path (if applicable)
  - Metaclass (initially "unknown")
  - Physics handle
- **Relationships**: List of SpatialRelationship objects, each containing:
  - Source and target entity IDs
  - Relationship type (contains, adjacent_to, above, etc.)
  - Confidence score
- **Metadata**: Version, timestamp, processing time, and statistics

## Technology Stack

- **Language**: C++20
- **Core Libraries**:
  - Eigen for linear algebra and vector operations
  - Standard C++ libraries for data structures and algorithms
- **Build System**: CMake
- **Testing Framework**: Built-in test infrastructure

## Installation and Usage

### Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/AME-Scanner.git
   cd AME-Scanner
   ```

2. Create build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure CMake:
   ```bash
   cmake ..
   ```

4. Build the project:
   ```bash
   cmake --build . --config Release
   ```

### Using scanner-cli

```bash
# Basic usage
scanner-cli input_file.ply output.ssp

# Using custom parameters
scanner-cli --epsilon 0.15 --min-pts 10 input_file.splat output.ssp

# Enable verbose output
scanner-cli --verbose input_file.ply output.ssp
```

## Examples

### Processing 3DGS Data and Generating Spatial Structure Package

```cpp
#include "field_loader.h"
#include "dbscan.h"
#include "spatial_structure_package.h"

int main() {
    // Load 3DGS data
    AmeScanner::FieldLoader loader;
    std::vector<AmeScanner::Gaussian> gaussians;
    loader.loadFromPLY("input.ply", gaussians);
    
    // Perform DBSCAN clustering
    AmeScanner::DBSCAN dbscan(0.1f, 5);
    auto clusters = dbscan.cluster(gaussians);
    
    // Create SpatialStructurePackage
    AmeScanner::SpatialStructurePackage ssp;
    // ... populate data ...
    
    // Serialize output
    ssp.serialize("output.ssp");
    
    return 0;
}
```

## Verification Plan

AME Scanner project includes a detailed verification plan to ensure all core functions work correctly. The verification plan includes:

- 3DGS data loading verification
- DBSCAN clustering algorithm verification
- Density field analysis verification
- Surface candidate extraction verification
- Spatial relationship graph construction verification
- SpatialStructurePackage output verification
- scanner-cli command line tool verification
- Integration testing
- Performance testing
- Code quality verification

Detailed verification plan can be found in the `test-preparation` directory.

## Contribution Guidelines

We welcome community contributions! If you want to contribute to the AME Scanner project, please:

1. Fork the repository
2. Create a new branch
3. Implement your changes
4. Write tests
5. Submit a pull request

## License

AME Scanner project is licensed under the MIT License. See the LICENSE file for details.

## Contact

- Project Home: https://github.com/yourusername/AME-Scanner
- Issue Tracking: https://github.com/yourusername/AME-Scanner/issues

---

*Oasis Company / AMAR Engine Project*
**Direction defines miracles.**
