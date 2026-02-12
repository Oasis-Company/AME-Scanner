# PROTOCOL: THE SPATIAL HANDSHAKE (SAP-01)

## 1. MISSION STATEMENT

The **AME Scanner** is an autonomous sensory organ.  
Its objective is not *reconstruction*—but **deconstruction**.

It converts raw **Gaussian Splatting (GS) radiance fields** into structured **Spatial Primitives**.

This protocol—**Scanner Assignment Protocol 01 (SAP-01)**—defines the syntax by which the Scanner communicates **“Found Truth”** to the **AME Admin**.

---

## 2. THE LOGIC OF DISCOVERY

The Scanner operates on a **Late-Binding Semantic model**:  
it identifies **physical mass** before it understands **object identity**.

Communication follows a three-stage evolution:

### I. Volumetric Anchoring  
The Scanner isolates a high-density cluster within the radiance field and computes its **Oriented Bounding Box (OBB)**.  
This state is known as the **Primitive State**.

### II. Signature Extraction  
A **Visual DNA** is generated—comprising:  
- Localized feature hashes  
- Optimal vantage-point captures  

This signature encodes perceptual invariance across sessions.

### III. Payload Emission  
All data is packaged into a **high-speed, lightweight telemetry packet** and transmitted to the AME Admin for **Metaclass Injection**.

---

## 3. DATA ARCHITECTURE

An entity is defined by four primary dimensions:

| Dimension     | Attribute        | Description |
|---------------|------------------|-------------|
| **Spatial**   | `Centroid`       | Absolute `[x, y, z]` origin in global coordinates |
| **Volumetric**| `Extents`        | `[width, height, depth]` of the minimal OBB |
| **Kinetic**   | `Orientation`    | Quaternion-based rotation for non-axis-aligned entities |
| **Descriptor**| `FeatureHash`    | Unique signature for cross-session entity recognition |

---

## 4. TRANSMISSION SPECIFICATION

To enforce system decoupling, the Scanner emits an **Atomic Packet**.  
Upon receipt, the AME Admin initiates the **Semantic Bridge** to the **Metaclass Library**.

<details>
<summary><strong>View Telemetry Payload Schema</strong> (Technical Implementation Only)</summary>

```json
{
  "header": {
    "probe_id": "STRING",
    "timestamp": "ISO-8601"
  },
  "entity": {
    "aeid_alpha": "HEX_STRING",
    "transform": {
      "pos": [0.0, 0.0, 0.0],
      "rot": [0, 0, 0, 1],
      "scale": [1, 1, 1]
    },
    "field_data": {
      "density_avg": "FLOAT",
      "visual_dna": "HASH"
    }
  }
}
