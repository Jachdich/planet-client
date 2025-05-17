#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../include/treegen.h"
#define RANDOM_H_IMPLEMENTATION
#include "../include/random.h"

#define OCTREE_MAX_MEMBERS 10
#define LEAF_RAD 3
#define LENGTH 2

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    float x, y, z;
} Vec3;

struct TreeParams;
typedef struct TreeParams TreeParams;
struct TreeParams {
    float MAX_WIDTH;
    float BRANCH_BIAS;
    float BIAS_STRENGTH;
    float MAX_STRAIGHT_CHANCE;
    float MIN_STRAIGHT_CHANCE;
    float MIN_TRUNK_WIDTH;

    // Max chance to branch unequally i.e. branching off the trunk
    float MAX_BRANCH_CHANCE;

    // maximum angular deviations before being biased
    Vec2 MAX_DEVIATION;

    // minimum width of the lower trunk (before any branches). After the trunk is thinner than this, it will have a chance to branch.
    float MIN_LOWER_TRUNK_WIDTH;
    int MIN_LOWER_TRUNK_LENGTH; // minimum number of sections for the lower trunk to have

    // standard deviation of the random angle added to a new section on a straight branch
    Vec2 STRAIGHT_DEVIATION_STDDEV;

    // each new section along a straight branch will be this fraction of the previous width
    float STRAIGHT_WIDTH_TRUNK_MULTIPLIER, STRAIGHT_WIDTH_BRANCH_MULTIPLIER;

    // fork: Y shape.
    // azumuth difference - azimuth angle between the two new branches
    // elevation: change in elevation of the new branches (mean is added to current elevation)
    float FORK_AZIMUTH_DIFFERENCE_MEAN, FORK_AZIMUTH_DIFFERENCE_STDDEV;
    float FORK_ELEVATION_MEAN, FORK_ELEVATION_STDDEV;

    // multiplier to give the trunk and branch a bit more width after dividing, as the sum of the thickness of two branches is often greater than the thickness of the original
    float BRANCH_EXTRA;
    float TRUNK_EXTRA;

    // chance, based on width, to go straight, rather than branching
    float (*STRAIGHT_CHANCE)(float, bool, TreeParams*);

    // how squished the leaves look. < 1 -> tall and skinny, > 1 -> short and fat, = 1 -> spherical
    float LEAF_OVALNESS;

    // radius of leaf sphere(oid), picked randomly between these values
    float LEAF_MIN_RAD, LEAF_MAX_RAD;

    // how much the leaves can be pushed in and out randomly
    float LEAF_RAD_RANDOM_OFFSET;

    // maximum elevation of the leaves, in effect cutting off the leaf bundle from the bottom
    float LEAF_MAX_ELEVATION;
};

#define pi 3.14159265358979324

float oak_straight_chance(float width, bool istrunk, TreeParams *oak) {
    float f = (width/(sqrt(1/(oak->MAX_STRAIGHT_CHANCE - oak->MIN_STRAIGHT_CHANCE)) * oak->MAX_WIDTH));
    return f * f + oak->MIN_STRAIGHT_CHANCE;
}

TreeParams oak() {
    TreeParams oak;
    oak.MAX_WIDTH = 8;
    oak.BRANCH_BIAS = pi/4;
    oak.BIAS_STRENGTH = 0.6;
    oak.MAX_STRAIGHT_CHANCE = 0.97;
    oak.MIN_STRAIGHT_CHANCE = 0.6;
    oak.MIN_TRUNK_WIDTH = oak.MAX_WIDTH * 0.3;
    oak.MAX_BRANCH_CHANCE = 0;
    oak.MAX_DEVIATION = (Vec2){pi/6, pi/6};
    oak.MIN_LOWER_TRUNK_WIDTH = 0.9 * oak.MAX_WIDTH;
    oak.MIN_LOWER_TRUNK_LENGTH = 10;
    oak.STRAIGHT_DEVIATION_STDDEV = (Vec2){pi/18, pi/18};
    oak.STRAIGHT_WIDTH_TRUNK_MULTIPLIER = 0.98;
    oak.STRAIGHT_WIDTH_BRANCH_MULTIPLIER = 0.98;
    oak.FORK_AZIMUTH_DIFFERENCE_MEAN = pi;
    oak.FORK_AZIMUTH_DIFFERENCE_STDDEV = 2*pi/2;
    oak.FORK_ELEVATION_MEAN = pi/7;
    oak.FORK_ELEVATION_STDDEV = pi/4;
    oak.BRANCH_EXTRA = 1.2;
    oak.TRUNK_EXTRA = 1.2;
    oak.STRAIGHT_CHANCE = &oak_straight_chance;
    oak.LEAF_OVALNESS = 1;
    oak.LEAF_MIN_RAD = 4;
    oak.LEAF_MAX_RAD = 10;
    oak.LEAF_RAD_RANDOM_OFFSET = 4;
    oak.LEAF_MAX_ELEVATION = 3*pi/5;
    return oak;
}


float poplar_straight_chance(float width, bool istrunk, TreeParams *polar) {
    return istrunk ? 0.2 : 0.9; 
}

TreeParams poplar() {
    TreeParams tree;
    tree.MAX_WIDTH = 8;
    tree.BRANCH_BIAS = pi/8;
    tree.BIAS_STRENGTH = 0.6;
    tree.MAX_STRAIGHT_CHANCE = 0.9;
    tree.MIN_STRAIGHT_CHANCE = 0.5;
    tree.MIN_TRUNK_WIDTH = tree.MAX_WIDTH * 0.3;
    tree.MAX_BRANCH_CHANCE = 5;
    tree.MAX_DEVIATION = (Vec2){pi/10, pi/10};
    tree.MIN_LOWER_TRUNK_WIDTH = 0.9 * tree.MAX_WIDTH;
    tree.MIN_LOWER_TRUNK_LENGTH = 10 ;
    tree.STRAIGHT_DEVIATION_STDDEV = (Vec2){pi/26, pi/26};
    tree.STRAIGHT_WIDTH_TRUNK_MULTIPLIER = 0.98;
    tree.STRAIGHT_WIDTH_BRANCH_MULTIPLIER = 0.8;
    tree.FORK_AZIMUTH_DIFFERENCE_MEAN = pi;
    tree.FORK_AZIMUTH_DIFFERENCE_STDDEV = 2*pi/2;
    tree.FORK_ELEVATION_MEAN = pi/7;
    tree.FORK_ELEVATION_STDDEV = pi/4;
    tree.BRANCH_EXTRA = 5;
    tree.TRUNK_EXTRA = 1.1;
    tree.STRAIGHT_CHANCE = poplar_straight_chance;
    tree.LEAF_OVALNESS = 0.3;
    tree.LEAF_MIN_RAD = 6;
    tree.LEAF_MAX_RAD = 12;
    tree.LEAF_RAD_RANDOM_OFFSET = 2;
    tree.LEAF_MAX_ELEVATION = pi;
    return tree;
}

Vec2 vec2_add(Vec2 this, Vec2 other) {
    this.x += other.x;
    this.y += other.y;
    return this;
}

Vec2 vec2_mul(Vec2 this, float f) {
    this.x *= f;
    this.y *= f;
    return this;
}

Vec2 from_vec3(Vec3 v) {
    return (Vec2) { .x = v.x, .y = v.y };
}


Vec3 from_vec2(Vec2 v) {
    return (Vec3){.x = v.x, .y = v.y, .z = 0};
}

Vec3 vec3_add(Vec3 this, Vec3 other) {
    this.x += other.x;
    this.y += other.y;
    this.z += other.z;
    return this;
}

Vec3 vec3_mul(Vec3 this, float f) {
    this.x *= f;
    this.y *= f;
    this.z *= f;
    return this;
}

bool vec3_eq(Vec3 this, Vec3 other) {
    return this.x == other.x && this.y == other.y && this.z == other.z;
}

Vec3 spherical(Vec2 angles) {
    return (Vec3) {
        .x = sin(angles.x) * cos(angles.y),
        .y = sin(angles.x) * sin(angles.y),
        .z = cos(angles.x)
    };
}

Vec3 rotateX(Vec3 coords, float rad) {
    float cosa = cos(rad);
    float sina = sin(rad);
    float y = coords.y * cosa - coords.z * sina;
    float z = coords.y * sina + coords.z * cosa;
    return (Vec3) {coords.x, y, z};
}

Vec3 rotateY(Vec3 coords, float rad) {
    float cosa = cos(rad);
    float sina = sin(rad);
    float z = coords.z * cosa - coords.x * sina;
    float x = coords.z * sina + coords.x * cosa;
    return (Vec3){x, coords.y, z};
}

Vec3 rotateZ(Vec3 coords, float rad) {
    float cosa = cos(rad);
    float sina = sin(rad);
    float x = coords.x * cosa - coords.y * sina;
    float y = coords.x * sina + coords.y * cosa;
    return (Vec3){x, y, coords.z};
}

Arena arena_new(size_t max_bytes) {
    return (Arena) {
        .buffer = malloc(max_bytes),
        .pos = 0,
        .cap = max_bytes,
    };
}

void *arena_alloc(Arena *a, size_t n_bytes) {
    // printf("Alloc %lu bytes\n", n_bytes);
    if (n_bytes >= a->cap - a->pos) {
        fprintf(stderr, "Attempt to allocate %lu bytes: not enough space\n", n_bytes);
        return NULL;
    }
    void *b = a->buffer + a->pos;
    a->pos += n_bytes;
    return b;
    // return malloc(n_bytes);
}

struct Octree;
typedef struct Octree Octree;
struct Octree {
    Octree *children;
    Vec3 points[OCTREE_MAX_MEMBERS];
    size_t n_points;
    Vec3 centre;
    float rad;
    bool leaf;
};

void octree_init(Octree *self, Vec3 origin, float rad) {
    self->centre = origin;
    self->rad = rad;
    self->leaf = true;
    self->n_points = 0;
}

void octree_subdivide(Octree *self, Arena *a);
void octree_add(Octree *self, Vec3 point, Arena *a) {
    if (self->leaf) {
        self->points[self->n_points] = point;
        self->n_points += 1;
        if (self->n_points == OCTREE_MAX_MEMBERS) {
            octree_subdivide(self, a);
        }
    } else {
        int top   = (point.x > self->centre.x) ? 1 : 0;
        int left  = (point.y > self->centre.y) ? 1 : 0;
        int front = (point.z > self->centre.z) ? 1 : 0;
        int index = top << 2 | left << 1 | front;
        octree_add(&self->children[index], point, a);
    }
}

void octree_subdivide(Octree *self, Arena *a) {
    self->leaf = false;
    float r = self->rad / 2;
    self->children = arena_alloc(a, 8 * sizeof(Octree));
    for (int i = 0; i < 8; i++) {
        int top =   ((i >> 2) & 1) * 2 - 1;
        int left =  ((i >> 1) & 1) * 2 - 1;
        int front = ((i >> 0) & 1) * 2 - 1;
        Vec3 new_origin = vec3_add(self->centre, (Vec3){.x = r * top, .y = r * left, .z = r * front});
        octree_init(&self->children[i], new_origin, r);
    }

    for (int i = 0; i < self->n_points; i++) {
        octree_add(self, self->points[i], a);
    }
    self->n_points = 0;
}

int octree_query(Octree *self, Vec3 pos) {
    if (self->leaf) {
        int count = 0;
        for (int i = 0; i < self->n_points; i++) {
            Vec3 point = self->points[i];
            if (vec3_eq(point, pos)) continue;
            float dist = (point.x - pos.x) * (point.x - pos.x) +
                         (point.y - pos.y) * (point.y - pos.y) +
                         (point.z - pos.z) * (point.z - pos.z);
            if (dist < LEAF_RAD * LEAF_RAD)
                count += 1;
        }
        return count;
    }

    int top   = (pos.x > self->centre.x) ? 1 : 0;
    int left  = (pos.y > self->centre.y) ? 1 : 0;
    int front = (pos.z > self->centre.z) ? 1 : 0;
    int index = top << 2 | left << 1 | front;
    return octree_query(&self->children[index], pos);
}

struct Section;
typedef struct Section Section;
struct Section {
    float width;
    Vec3 pos;
    Vec2 angles;
    Section *children;
    size_t n_children;
    Vec2 bias;
    Vec3 end_pos;
    int inarow;
    bool is_trunk;
    TreeParams *params;
};

void section_init(Section *s, float width, Vec3 pos, Vec2 angles, TreeParams *params) {
    s->width = width;
    s->pos = pos;
    s->angles = angles;
    s->bias = (Vec2){0, 0}; // TODO
    s->n_children = 0;
    s->end_pos = vec3_add(vec3_mul(spherical(angles), LENGTH), pos);
    s->params = params;
}

static PRNG32RomuQuad prng32;

float random_float() {
    return dist_uniformf_dense(0, 1, prng32_romu_quad, &prng32);
}

float random_gauss(float mu, float sigma) {
    return dist_normalf(prng32_romu_quad, &prng32) * sigma + mu;
}

float random_uniform(float lower, float upper) {
    return dist_uniformf_dense(lower, upper, prng32_romu_quad, &prng32);
}

typedef struct {
    Vec2 angle_0, angle_1;
    float width_0, width_1;
    Vec2 bias_0, bias_1;
    bool trunk_0, trunk_1;
} AWBT;

void branch(Section *self, Vec2 bias_input, Octree *oct, Arena *a);
void go_straight(Section *self, Vec2 bias_input, Octree *oct, Arena *a);
AWBT branch_off_trunk(Section *self, Vec2 bias_input);
AWBT branch_equally(Section *self);
void make_leaves(TreeParams *TT, Octree *oct, Vec3 bush_pos, Arena *arena);

void tree(Section *self, int inarow, Vec2 bias, bool is_trunk, Octree *oct, Arena *a) {
    self->bias = bias;
    self->inarow = inarow;
    self->is_trunk = is_trunk;
    if (self->width < 1) {
        make_leaves(self->params, oct, self->pos, a);
        return;
    }

    float bias_factor = fabs(self->angles.x - bias.x);
    Vec2 bias_input = (Vec2){0, 0};
    if (bias_factor < self->params->MAX_DEVIATION.x) {
        bias_input.x = 0;
    } else if (self->angles.x > bias.x) {
        bias_input.x = -bias_factor * self->params->BIAS_STRENGTH;
    } else {
        bias_input.x = bias_factor * self->params->BIAS_STRENGTH;
    }

    if (!isnan(bias.y)) {
        bias_factor = fabs(self->angles.y - bias.y);
        if (bias_factor < self->params->MAX_DEVIATION.y) {
            bias_input.y = 0;
        } else if (self->angles.y > bias.y) {
            bias_input.y = -bias_factor * self->params->BIAS_STRENGTH;
        } else {
            bias_input.y = bias_factor * self->params->BIAS_STRENGTH;
        }
    }

    float straight_chance = self->params->STRAIGHT_CHANCE(self->width, is_trunk, self->params);

    // make the trunk longer;
    if (self->width >= self->params->MIN_LOWER_TRUNK_WIDTH && inarow < self->params->MIN_LOWER_TRUNK_LENGTH) {
        straight_chance = 1;
    }
    
    float n = self->params->MAX_WIDTH;
    // TODO make parameter;
    if (random_float() > straight_chance || inarow > (-5/(n-1)*self->width + 5/(n-1)*n + 8)) {
        branch(self, bias_input, oct, a);
    } else {
        go_straight(self, bias_input, oct, a);
    }

}
Vec2 straight_branch_angles(Section *self, Vec2 bias_input) {
    return (Vec2){
        self->angles.x + random_gauss(bias_input.x, self->params->STRAIGHT_DEVIATION_STDDEV.x),
        self->angles.y + random_gauss(bias_input.y, self->params->STRAIGHT_DEVIATION_STDDEV.y),
    };
}

void go_straight(Section *self, Vec2 bias_input, Octree *oct, Arena *a) {
    Vec2 angles = straight_branch_angles(self, bias_input);
    self->children = arena_alloc(a, sizeof(Section));
    self->n_children = 1;
    section_init(self->children, self->width * (self->is_trunk ? self->params->STRAIGHT_WIDTH_TRUNK_MULTIPLIER : self->params->STRAIGHT_WIDTH_BRANCH_MULTIPLIER), self->end_pos, angles, self->params);
    tree(self->children, self->inarow + 1, self->bias, self->is_trunk, oct, a);
}

void branch(Section *self, Vec2 bias_input, Octree *oct, Arena *a) {
    float branch_chance;
    if (self->width > self->params->MIN_TRUNK_WIDTH) {
        branch_chance = (log(self->width-self->params->MIN_TRUNK_WIDTH)/log(self->params->MAX_WIDTH-self->params->MIN_TRUNK_WIDTH)+1)/2 * self->params->MAX_BRANCH_CHANCE;
    } else {
        branch_chance = 0;
    }

    AWBT awbt;
    if (random_float() < branch_chance && self->is_trunk) {
        awbt = branch_off_trunk(self, bias_input);
    } else {
        awbt = branch_equally(self);
    }
    self->children = arena_alloc(a, 2 * sizeof(Section));
    self->n_children = 2;
    section_init(&self->children[0], awbt.width_0, self->end_pos, awbt.angle_0, self->params);
    section_init(&self->children[1], awbt.width_1, self->end_pos, awbt.angle_1, self->params);
    tree(&self->children[0], 0, awbt.bias_0, awbt.trunk_0, oct, a);
    tree(&self->children[1], 0, awbt.bias_1, awbt.trunk_1, oct, a);
}

AWBT branch_off_trunk(Section *self, Vec2 bias_input) {
    AWBT awbt;
    awbt.angle_0 = straight_branch_angles(self, bias_input);
    awbt.angle_1 = (Vec2){
            self->angles.x + random_uniform(pi/8, pi/2 - self->angles.x),
            self->angles.y + random_uniform(-pi, pi),
        };
    float main_width = random_uniform(self->width * 0.85, self->width * 0.86);
    awbt.width_0 = main_width * self->params->TRUNK_EXTRA;
    awbt.width_1 = (self->width - main_width) * self->params->BRANCH_EXTRA;

    awbt.bias_0 = (Vec2){0, NAN};
    awbt.bias_1 = (Vec2){self->params->BRANCH_BIAS, awbt.angle_1.y};
    awbt.trunk_0 = true;
    awbt.trunk_1 = false;
    return awbt;
}

Vec2 clamp(Vec2 x) {
    if (x.x > pi/4) {
        x.x = pi/4;
    }
    if (x.x < -pi/4) {
        x.x = -pi/4;
    }
    return x;
}

AWBT get_fork_angles(Section *self) {
    // make sure branches are somewhat different direction to eachother;
    // TODO huh
    float azimuth_difference = self->params->FORK_AZIMUTH_DIFFERENCE_MEAN; //random_uniform(self->params->FORK_AZIMUTH_DIFFERENCE_MEAN, self->params->FORK_AZIMUTH_DIFFERENCE_STDDEV);
    float azimuth_a = random_uniform(0, 2*pi);
    float azimuth_b = azimuth_a + azimuth_difference;

    float elevation_a = fabs(random_gauss(self->angles.x + self->params->FORK_ELEVATION_MEAN, self->params->FORK_ELEVATION_STDDEV));
    float elevation_b = fabs(random_gauss(self->angles.x + self->params->FORK_ELEVATION_MEAN, self->params->FORK_ELEVATION_STDDEV));

    AWBT awbt;
    awbt.angle_0 = (Vec2){elevation_a, azimuth_a};
    awbt.angle_1 = (Vec2){elevation_b, azimuth_b};
    return awbt;
}

AWBT branch_equally(Section *self) {
    AWBT awbt;
    if (self->angles.x < pi/8) {
        AWBT b = get_fork_angles(self);
        awbt.angle_0 = b.angle_0;
        awbt.angle_1 = b.angle_1;
    } else {
            awbt.angle_0 = (Vec2) {
                self->angles.x + random_gauss(pi/16, pi/32),
                self->angles.y + random_gauss(3*pi/8, pi/10),
            };
            awbt.angle_1 = (Vec2) {
                self->angles.x - random_gauss(pi/16, pi/32),
                self->angles.y - random_gauss(3*pi/8, pi/10),
            };
    }

    if (self->is_trunk && self->width > self->params->MIN_TRUNK_WIDTH) {
        awbt.bias_0 = (Vec2){awbt.angle_0.x, NAN};
        awbt.bias_1 = (Vec2){awbt.angle_1.x, NAN};
        awbt.trunk_0 = true;
        awbt.trunk_1 = true;
    } else {
        awbt.bias_0 = (Vec2){self->params->BRANCH_BIAS, awbt.angle_0.y};
        awbt.bias_1 = (Vec2){self->params->BRANCH_BIAS, awbt.angle_1.y};
        awbt.trunk_0 = false;
        awbt.trunk_1 = false;
    }

    awbt.bias_0 = clamp(awbt.bias_0);
    awbt.bias_1 = clamp(awbt.bias_1);

    float width = random_gauss(0.5, 0.1) * self->width;
    awbt.width_0 = width * self->params->TRUNK_EXTRA;
    awbt.width_1 = (self->width - width) * self->params->TRUNK_EXTRA;
    return awbt;
}

typedef struct {
    uint32_t col;
    float z;
} Pixel;

#define resolution 64
const Vec2 ORIGIN = (Vec2) {32, 50};
const Vec3 LIGHT_DIR = (Vec3){0, 0.7071067811865475, 0.7071067811865475};
const uint32_t TRUNK_COLOURS[] = {
   0x353130, 0x4d403d, 0x64534b, 0x8a6b58, 0xb0945e,
};
const size_t N_TRUNK_COLOURS = 5;

const float MAX_HUE_SHIFT = 0.2;
const int NUM_COLOURS = 5;
const float MIN_VALUE = 0.2;
const float MAX_VALUE = 0.9;

float qerp(float a, float b, float c, float t) {
    return powf(1-t, 2)*a+2*(1-t)*t*b+powf(t, 2)*c;
}

// uint32_t *generate_palette(uint32_t init_hue, Arena *a) {
//     uint32_t *palette = arena_alloc(a, NUM_COLOURS * sizeof(*palette));
//     for (int i = 0; i < NUM_COLOURS; i++) {
//         float hue_shift = qerp(MAX_HUE_SHIFT, -MAX_HUE_SHIFT, -MAX_HUE_SHIFT, i/(NUM_COLOURS-1));
//         value = lerp(MIN_VALUE, MAX_VALUE, i/(NUM_COLOURS-1));
//         saturation = qerp(0.2, 0.5, 0.5, i/(NUM_COLOURS-1));
//         r,g,b = colorsys.hsv_to_rgb(init_hue + hue_shift, saturation, value);
//         palette[i] = ([int(r * 255), int(g * 255), int(b * 255)])
//     }
//     return palette
// }

// LEAF_COLOURS = generate_palette(0.38)

const int LEAF_COLOURS[] = {
    0x19332d,
    0x25562e,
    0x468232,
    0x75a743,
    0xa8ca58,
};

float raycast(Vec3 pos, Octree *leaf_octree) {
    Vec3 ray = vec3_add((Vec3){pos.x, pos.y, pos.z}, vec3_mul(LIGHT_DIR, LEAF_RAD));
    float light = 1.0;
    for (int i = 0; i < 30; i++) {
        int leaves_hit = octree_query(leaf_octree, ray);
        if (leaves_hit > 0) {
            light *= powf(0.98, leaves_hit);
        }
        ray = vec3_add(ray, LIGHT_DIR);
    }
    return light;
}

void draw(Section *self, Vec2 ang, Pixel *buf, Octree *oct) {
    for (int n = 0; n < LENGTH; n++) {
        Vec3 end_pos = vec3_add(vec3_mul(spherical(self->angles), n), self->pos);
        Vec3 end_pos_ss = rotateX(rotateZ(end_pos, ang.y), ang.x);
        float actual_width = round(self->width / self->params->MAX_WIDTH * 2.8) + 1;

        for (int dx = 0; dx < actual_width; dx++) {
            int dy = 0;
            int x = (end_pos_ss.x + dx - actual_width/2) + ORIGIN.x;
            int y = (end_pos_ss.y + dy - actual_width/2) + ORIGIN.y;
            float depth = buf[y * resolution + x].z;
            if (end_pos_ss.z < depth) {
                Vec3 pixel_ws = rotateZ(rotateX(vec3_add((Vec3){x, y, end_pos_ss.z}, (Vec3){-ORIGIN.x, -ORIGIN.y, 0}), -ang.x), -ang.y);
        
                float light = raycast(pixel_ws, oct);
                int col = TRUNK_COLOURS[(size_t)(light * (N_TRUNK_COLOURS) - 1)];
                buf[y*resolution+x] = (Pixel){col, end_pos_ss.z};
            }
        }
    }
    for (size_t i = 0; i < self->n_children; i++) {
        draw(&self->children[i], ang, buf, oct);
    }
}

void make_leaves(TreeParams *TT, Octree *oct, Vec3 bush_pos, Arena *arena) {
    float max_radius = random_uniform(TT->LEAF_MIN_RAD, TT->LEAF_MAX_RAD);
    float max_elevation = TT->LEAF_MAX_ELEVATION;
    float a = 1;
    float b = 1;
    float c = TT->LEAF_OVALNESS;
    for (float elevation = 0; elevation < max_elevation; elevation += 0.1) {
        if (elevation > pi) continue;
        float x = elevation;
        int points_per_circle = 45;
        float d = pi/2;
        int num_points = b*(1-powf(((x-d)/d), 2)) * points_per_circle;
        if (num_points == 0) {
            num_points = 1;
        }
        float step = 2*pi/num_points;
        for (float azimuth = -pi; azimuth < pi; azimuth += step) {
            float γ = elevation;
            float λ = azimuth;
            float actual_radius = a*b*c/sqrt(c*c*(b*b*powf(cos(λ),2)+a*a*powf(sin(λ),2))*powf(cos(γ),2) + a*a*b*b*powf(sin(γ),2)) * max_radius;
            float radius = actual_radius;
            Vec3 dir = spherical((Vec2){elevation, azimuth});
            Vec3 offset = vec3_mul(dir, radius);
            Vec3 pos = vec3_add(bush_pos, offset);
            octree_add(oct, pos, arena);
        }
    }
}

void draw_leaf(Vec3 leaf, Vec2 ang, Octree *octree, Pixel *buf) {
    Vec3 pos = rotateX(rotateZ(leaf, ang.y), ang.x);
    int idx = (int)(pos.y + ORIGIN.y) * resolution + (int)(pos.x + ORIGIN.x);
    if (pos.z <= buf[idx].z) {
        float light = raycast(leaf, octree);
        if (light != 1) {
            printf("%f\n", light);
        }
        uint32_t col = LEAF_COLOURS[(int)(light * (NUM_COLOURS - 1))];
        buf[idx] = (Pixel){col, pos.z};
    }
}

void draw_leaves(Octree *tree, Octree *fulltree, Vec2 ang, Pixel *buf) {
    if (tree->leaf) {
        for (int i = 0; i < tree->n_points; i++) {
            draw_leaf(tree->points[i], ang, fulltree, buf);
        }
    } else {
        for (int i = 0; i < 8; i++) {
            draw_leaves(&tree->children[i], fulltree, ang, buf);
        }
    }
}

void make_tree(Arena *a, uint32_t *out_buffer) {
    prng32_romu_quad_randomize(&prng32);
    Vec2 ang = (Vec2){9*pi/16, 11*pi/16};
    Pixel buf[resolution * resolution];
    for (int i = 0; i < resolution * resolution; i++) {
        buf[i].z = 255;
        buf[i].col = 0x000000;
    }
    TreeParams tt = oak();
    Octree octree;
    octree_init(&octree, (Vec3){0, 0, 40}, 40);
    Section root;
    section_init(&root, tt.MAX_WIDTH, (Vec3){0.0, 0.0, 0.0}, (Vec2){0.0, 0.0}, &tt);
    tree(&root, 0, (Vec2){0, NAN}, true, &octree, a);
    draw(&root, ang, buf, &octree);
    draw_leaves(&octree, &octree, ang, buf);
    for (int y = 0; y < resolution; y++) {
        for (int x = 0; x < resolution; x++) {
            out_buffer[y * resolution + x] = buf[y * resolution + x].col;
            
        }
    }
}
