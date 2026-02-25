# AME Scanner: Project Overview

## 1. Project Introduction

AME Scanner is a critical component of the AMAR Engine ecosystem, designed to process 3D Gaussian Splatting (3DGS) data and convert it into structured spatial information for the Information Pool. The scanner plays a crucial role in the multi-path world construction pipeline, providing raw structural data that feeds into the AME Admin's metaclass inference process.

## 2. Architecture Integration

### Position in AMAR Engine Architecture

```
Input Layer
↓
Information Pool ← [AME Scanner Output]
↓
AME Admin (Metaclass Inference)
↓
AMAR WORLD (Persistent Standard)
↓
GENIS Runtime Adapter
↓
Genesis Physics Core
```

The scanner operates exclusively within the Input Layer, generating raw structured information without performing any metaclass inference. This strict separation ensures the integrity of the AMAR Engine's layered architecture.

## 3. Development Phases

The scanner development is divided into 5 distinct phases:

### Phase 1: 3DGS Reading and Parsing
- **Goal**: Establish a stable 3DGS reader
- **Tasks**:
  - Parse .ply / .splat / custom formats
  - Read position, covariance matrix, opacity, and color data
  - Convert to internal Gaussian data structure
- **Output**:
  ```python
  class Gaussian:
      position: Vec3
      covariance: Mat3
      color: Vec3
      opacity: float
  ```
- **Completion Criteria**: Successfully load a 3DGS field and visualize point distribution
- **Estimated Time**: 1-2 days

### Phase 2: Spatial Density Field Analysis
- **Goal**: Build continuous density approximation from Gaussian collection
- **Tasks**:
  - Voxelization or KD-Tree based local density query
  - Calculate local density gradients, sparse regions, and high-density regions
- **Output**: density_field, gradient_field
- **Completion Criteria**: Ability to output density heatmap
- **Estimated Time**: 2-3 days

### Phase 3: Clustering and Region Separation
- **Goal**: Split continuous field into candidate structural blocks
- **Methods**: DBSCAN (density-based), Mean Shift, or spatial connectivity
- **Output**:
  ```python
  clusters = [
    {gaussian_ids: [...]},
    {gaussian_ids: [...]},
    ...
  ]
  ```
- **Completion Criteria**: 3DGS split into several spatial blocks (no naming, no classification, only编号)
- **Estimated Time**: 2-3 days

### Phase 4: Surface Candidate Extraction
- **Goal**: Identify potential "boundary surfaces"
- **Methods**: Local PCA, normal estimation, curvature analysis
- **Output**:
  ```python
  surface_candidates = [
    {
      cluster_id,
      normal_vector,
      area_estimate,
      curvature_stats
    }
  ]
  ```
- **Completion Criteria**: Geometric statistics without semantic interpretation
- **Estimated Time**: 3-4 days

### Phase 5: Spatial Relationship Graph Construction
- **Goal**: Generate spatial topology relationships
- **Identify**: Contact, adjacency, containment, support (based on Z-axis and contact area)
- **Output**:
  ```python
  spatial_graph = {
    nodes: cluster_ids,
    edges: [
      (A, B, relation_type, metrics)
    ]
  }
  ```
- **Completion Criteria**: Pure spatial relationship graph output
- **Estimated Time**: 3 days

## 4. Final Output Format

The scanner produces a **Raw Spatial Structure Package** in JSON format:

```json
{
  "clusters": [...],
  "density_stats": {...},
  "surface_candidates": [...],
  "spatial_relations": [...]
}
```

This output is designed to be:
- Free of metaclasses
- Free of semantics
- Free of object naming
- Purely geometric and statistical

## 5. Technical Architecture

### Three-Tier Structure

1. **scanner-core** (True Core)
   - **Responsibilities**:
     - Read 3DGS data
     - Density analysis
     - Clustering
     - Surface candidate extraction
     - Spatial relationship graph construction
   - **Dependencies**: None (pure spatial analysis library)
   - **Directory Structure**:
     ```
     scanner-core/
       src/
         io/
           g3ds_loader.py
         density/
           voxelizer.py
         clustering/
           dbscan.py
         surface/
           pca_surface.py
         graph/
           spatial_graph.py
         models/
           gaussian.py
           cluster.py
       tests/
     ```
   - **Output**: SpatialStructurePackage (standard data structure)

2. **scanner-cli** (Offline Tool)
   - **Responsibilities**:
     - Read 3DGS files
     - Call scanner-core
     - Output JSON
   - **Usage**:
     ```
     scanner input.splat --output pool.json
     ```

3. **scanner-engine-adapter** (Engine Embedded Version)
   - **Responsibilities**:
     - Convert SpatialStructurePackage to Information Pool format
     - Directly inject into AME Admin
   - **Dependencies**: scanner-core

### Technology Stack

- **Language**: Python (prototype phase)
- **Libraries**:
  - numpy
  - scipy
  - sklearn (DBSCAN)
  - open3d (point cloud processing)
  - pyvista (visualization)

## 6.