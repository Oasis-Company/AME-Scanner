# AME Scanner: Project Overview

## Project Overview

AME Scanner is an autonomous sensory component of the AMAR Engine ecosystem, designed to convert raw Gaussian Splatting (3DGS) radiance fields into structured spatial data. It serves as a critical input layer for the AMAR Engine, providing raw structured information to the Information Pool for subsequent metaclass inference by the AME Admin.

## Project Positioning

Within the AMAR Engine architecture, AME Scanner occupies a distinct position:

```
Input Layer
↓
Information Pool
↓
AME Admin (Metaclass Inference)
↓
AMAR WORLD (Persistent Standard)
↓
GENIS Runtime Adapter
↓
Genesis Physics Core
```

As part of the Input Layer, AME Scanner generates raw structured information rather than finalized world structures. It operates strictly within the generative and descriptive domain, without performing any metaclass inference.

## Core Functions

AME Scanner performs several key operations:

1. **3DGS Data Loading**
   - Reads various Gaussian Splatting formats (.ply, .splat)
   - Extracts essential geometric properties (position, color, opacity, scale, rotation)
   - Converts data into internal representation using Gaussian class

2. **Spatial Density Analysis**
   - Constructs continuous density approximation from Gaussian distributions
   - Computes local density at arbitrary points
   - Identifies dense regions using density thresholding
   - Provides comprehensive density statistics

3. **Clustering and Region Segmentation**
   - Segments continuous fields into candidate structural blocks using DBSCAN algorithm
   - Outputs anonymous, unclassified spatial clusters
   - Supports configurable clustering parameters (epsilon, min points)

4. **Surface Candidate Extraction**
   - Identifies potential boundary surfaces based on curvature analysis
   - Performs local PCA and normal estimation for each Gaussian
   - Computes curvature statistics to distinguish surface regions
   - Groups surface candidates into connected regions

5. **Spatial Relationship Graph Construction**
   - Generates spatial topology relationships between entities
   - Identifies adjacency, containment, and relative position relationships
   - Creates a purely geometric relationship graph
   - Supports graph visualization in DOT format

## Technical Architecture

AME Scanner is structured in three components:

### 1. scanner-core
The core algorithmic component that performs:
- 3DGS loading and parsing via FieldLoader
- Density field analysis via DensityAnalyzer
- Clustering operations via DBSCAN
- Surface candidate extraction via SurfaceExtractor
- Spatial relationship graph construction via SpatialGraph

This component is designed as a pure spatial analysis library with no dependencies on other AMAR Engine components.

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

This output is purely geometric and contains no semantic information, maintaining the clean separation between information gathering and metaclass inference.

## Technology Stack

- **Language**: C++20
- **Core Libraries**:
  - Eigen for linear algebra and vector operations
  - Standard C++ libraries for data structures and algorithms
- **Build System**: CMake
- **Testing Framework**: Built-in test infrastructure

## Development Philosophy

AME Scanner adheres to several key principles:

1. **Semantic Neutrality**: The scanner does not perform any semantic inference, preserving the clean separation between information gathering and metaclass assignment.

2. **Modularity**: Core functionality is isolated in scanner-core, enabling independent development and deployment.

3. **Scalability**: The architecture supports both standalone operation and engine integration.

4. **Extensibility**: The design allows for future expansion with new algorithms and formats.

5. **Performance**: Focus on efficient processing of large 3DGS datasets using C++ and optimized algorithms.

6. **Robustness**: Comprehensive error handling and validation to ensure reliable operation.

## Strategic Importance

AME Scanner is a foundational component of the AMAR Engine ecosystem, providing the critical first step in converting raw sensor data into structured spatial information. Its ability to extract meaningful geometric relationships from unstructured point clouds enables the subsequent metaclass inference process, ultimately contributing to the creation of persistent, executable world specifications.

By maintaining strict separation between information gathering and semantic inference, AME Scanner ensures the integrity of the AMAR Engine's layered architecture, enabling portability, versionability, and modular backend replacement.

## Implementation Details

### Key Classes

1. **Gaussian**: Represents a single Gaussian splat with position, color, opacity, scale, and rotation properties
2. **FieldLoader**: Loads and saves 3DGS data in .ply and .splat formats
3. **DBSCAN**: Performs density-based clustering on Gaussian point clouds
4. **DensityAnalyzer**: Computes density fields and identifies dense regions
5. **SurfaceExtractor**: Extracts surface candidates based on normal and curvature analysis
6. **SpatialGraph**: Builds spatial relationship graphs between entities
7. **SpatialStructurePackage**: Represents the output structure with entities and relationships

### Workflow

1. **Data Loading**: Load 3DGS data from .ply or .splat file
2. **Preprocessing**: Convert data to internal Gaussian representation
3. **Clustering**: Apply DBSCAN to identify spatial clusters
4. **Density Analysis**: Compute density field and identify dense regions
5. **Surface Extraction**: Identify surface candidates based on curvature
6. **OBB Fitting**: Compute oriented bounding boxes for each cluster
7. **Relationship Analysis**: Build spatial relationship graph
8. **Output**: Serialize results to SpatialStructurePackage

## Future Enhancements

1. **Performance Optimization**: Implement parallel processing for large datasets
2. **Format Support**: Add support for additional 3DGS formats
3. **Advanced Clustering**: Implement hierarchical clustering for better structure detection
4. **Semantic Integration**: Add optional semantic segmentation capabilities
5. **Real-time Processing**: Optimize for real-time applications
6. **Visualization Tools**: Add more comprehensive visualization capabilities

AME Scanner is now fully implemented and ready for integration with the AMAR Engine ecosystem, providing a robust foundation for spatial structure analysis and processing.
