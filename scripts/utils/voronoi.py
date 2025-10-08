import numpy as np
from scipy.spatial import Voronoi

def voronoi_polygons(points, bbox=(0, 0, 1000, 1000)):
    """
    Compute bounded Voronoi polygons from points inside a rectangular box.
    bbox = (xmin, ymin, xmax, ymax)
    """
    # Mirror points around the bounding box to bound infinite regions
    xmin, ymin, xmax, ymax = bbox
    extra_points = []
    for x, y in points:
        extra_points.extend([
            (x, ymin - (ymax - ymin)), (x, ymax + (ymax - ymin)),
            (xmin - (xmax - xmin), y), (xmax + (xmax - xmin), y)
        ])
    all_points = np.vstack([points, extra_points])
    vor = Voronoi(all_points)

    polygons = []
    for i, region_index in enumerate(vor.point_region[:len(points)]):
        region = vor.regions[region_index]
        if -1 in region or len(region) == 0:
            continue
        polygon = [vor.vertices[j] for j in region]
        # Clip polygon into the bbox
        poly_clipped = [
            (min(max(x, xmin), xmax), min(max(y, ymin), ymax))
            for x, y in polygon
        ]
        polygons.append(poly_clipped)
    return polygons

# Example: 100 random seeds
points = np.random.randint(0, 1000, (100, 2))
polygons = voronoi_polygons(points)

# Format in JUCE C++ style
formatted = "static std::vector<std::vector<juce::Point<float>>> cellPolygons = {\n"
for poly in polygons:
    formatted += "    { "
    formatted += ", ".join([f"{{ {float(x):.1f}f, {float(y):.1f}f }}" for x, y in poly])
    formatted += " },\n"
formatted += "};"

print(formatted)  # preview

