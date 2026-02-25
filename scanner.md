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
   - Reads various Gaussian Splatting formats (.ply, .splat, custom formats)
   - Extracts essential geometric properties (position, covariance matrix, opacity, color)
   - Converts data into internal representation

2. **Spatial Density Analysis**
   - Constructs continuous density approximation from Gaussian distributions
   - Computes local density gradients
   - Identifies sparse and dense regions

3. **Clustering and Region Segmentation**
   - Segments continuous fields into candidate structural blocks
   - Uses density-based clustering algorithms
   - Outputs anonymous, unclassified spatial clusters

4. **Surface Candidate Extraction**
   - Identifies potential boundary surfaces
   - Performs local PCA and normal estimation
   - Computes curvature statistics

5. **Spatial Relationship Graph Construction**
   - Generates spatial topology relationships
   - Identifies adjacency, intersection, and support candidates
   - Creates a purely geometric relationship graph

## Technical Architecture

AME Scanner is structured in three components:

### 1. scanner-core
The core algorithmic component that performs:
- 3DGS loading and parsing
- Density field analysis
- Clustering operations
- Surface candidate extraction
- Spatial relationship graph construction

This component is designed as a pure spatial analysis library with no dependencies on other AMAR Engine components.

### 2. scanner-cli
An offline tool that:
- Reads 3DGS files
- Invokes scanner-core functionality
- Outputs structured JSON data

### 3. scanner-engine-adapter
An integration component that:
- Converts SpatialStructurePackage to Information Pool format
- Directly injects data into AME Admin

## Output Format

AME Scanner produces a SpatialStructurePackage containing:

```json
{
  "clusters": [...],
  "density_stats": {...},
  "surface_candidates": [...],
  "spatial_relations": [...]
}
```

This output is purely geometric and contains no semantic information, maintaining the clean separation between information gathering and metaclass inference.

## Technology Stack

- **Language**: Python (prototype phase)
- **Core Libraries**:
  - NumPy for numerical computations
  - SciPy for scientific algorithms
  - scikit-learn for clustering (DBSCAN)
  - Open3D for point cloud processing
  - PyVista for visualization

## Development Philosophy

AME Scanner adheres to several key principles:

1. **Semantic Neutrality**: The scanner does not perform any semantic inference, preserving the clean separation between information gathering and metaclass assignment.

2. **Modularity**: Core functionality is isolated in scanner-core, enabling independent development and deployment.

3. **Scalability**: The architecture supports both standalone operation and engine integration.

4. **Extensibility**: The design allows for future expansion with new algorithms and formats.

5. **Performance**: Focus on efficient processing of large 3DGS datasets.

## Strategic Importance

AME Scanner is a foundational component of the AMAR Engine ecosystem, providing the critical first step in converting raw sensor data into structured spatial information. Its ability to extract meaningful geometric relationships from unstructured point clouds enables the subsequent metaclass inference process, ultimately contributing to the creation of persistent, executable world specifications.

By maintaining strict separation between information gathering and semantic inference, AME Scanner ensures the integrity of the AMAR Engine's layered architecture, enabling portability, versionability, and modular backend replacement.
