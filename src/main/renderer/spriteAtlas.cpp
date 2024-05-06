/**

zlib License

(C) 2020 Andrew Krause

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

**/

#include "../pch.h"

#include "sprite.h"
#include "../misc/misc.h"
#include "../core/resourceManager.h"
#include "tga.h"

namespace AB {

class Node {
    public:
        Node(u32 x, u32 y, u32 w, u32 h) : x(x), y(y), w(w), h(h) {
            used = false;
            right = NULL;
            down = NULL;
        }

        u32 x, y, w, h;
        b8 used;
        Node *right;
        Node *down;
};

static std::vector<Sprite*> atlasSprites;
static Node *root;

extern ResourceManager<Sprite> sprites;

void addToAtlas(Sprite *sprite) {
    atlasSprites.push_back(sprite);
}

static b8 cmp(Sprite *s1, Sprite *s2) {
    return (max(s1->width, s1->height) > max(s2->width, s2->height));
}

static Node* splitNode(Node* node, u32 w, u32 h) {
    node->used = true;
    node->down = new Node(node->x, node->y + h, node->w, node->h - h);
    node->right = new Node(node->x + w, node->y, node->w - w, h);

    return node;
}

static Node* findNode(Node *root, u32 w, u32 h) {
    if (root->used) {
        Node* right = findNode(root->right, w, h);
        if (right) {
            return right;
        } else {
            return findNode(root->down, w, h);
        }
    } else if ((w <= root->w) && (h <= root->h)) {
        return root;
    }
    return NULL;
}

static Node* growDown(u32 w, u32 h) {
    Node* oldRoot = root;

    root = new Node(0, 0, oldRoot->w, oldRoot->h + h);
    root->used = true;
    root->down = new Node(0, oldRoot->h, oldRoot->w, h);
    root->right = oldRoot;

    Node *node = findNode(root, w, h);
    if (node) {
        return splitNode(node, w, h);
    } else {
        return NULL;
    }
}

static Node* growRight(u32 w, u32 h) {
    Node* oldRoot = root;

    root = new Node(0, 0, oldRoot->w + w, oldRoot->h);
    root->used = true;
    root->down = oldRoot;
    root->right = new Node(oldRoot->w, 0, w, oldRoot->h);

    Node *node = findNode(root, w, h);
    if (node) {
        return splitNode(node, w, h);
    } else {
        return NULL;
    }
}

static Node* growNode(Node *root, u32 w, u32 h) {
    b8 canGrowDown = (w <= root->w);
    b8 canGrowRight = (w <= root->w);

    b8 shouldGrowRight = canGrowRight && (root->h >= (root->w + w));
    b8 shouldGrowDown = canGrowDown && (root->w >= (root->h + h));

    if (shouldGrowRight) {
        return growRight(w, h);
    } else if (shouldGrowDown) {
        return growDown(w, h);
    } else if (canGrowRight) {
        return growRight(w, h);
    } else if (canGrowDown) {
        return growDown(w, h);
    } else {
        return NULL;
    }
}

static void fit() {
    for (std::vector<Sprite*>::iterator sprite = atlasSprites.begin(); sprite != atlasSprites.end(); sprite++) {
        Node *node = findNode(root, (*sprite)->width, (*sprite)->height);
        if (node) {
            Node* fit = splitNode(node, (*sprite)->width, (*sprite)->height);
            (*sprite)->atlasX = fit->x;
            (*sprite)->atlasY = fit->y;
        } else {
            Node* fit = growNode(root, (*sprite)->width, (*sprite)->height);
            (*sprite)->atlasX = fit->x;
            (*sprite)->atlasY = fit->y;
        }
    }
}

static void deleteTree(Node* node) {
    if (node) {
        deleteTree(node->right);
        deleteTree(node->down);

        delete node;
        node = NULL;
    }
}

void buildAtlas() {
    //  sort sprites based on longest edge
    std::sort(atlasSprites.begin(), atlasSprites.end(), cmp);

    //  initialize the target to the size of the largest sprite
    root = new Node(0, 0, atlasSprites.at(0)->width, atlasSprites.at(0)->height);
    fit();
    u32 atlasWidth = root->w;
    u32 atlasHeight = root->h;
    deleteTree(root);

    LOG_EXP(atlasWidth);
    LOG_EXP(atlasHeight);

    //  create atlas texture width root->w, root->h size
    atlasWidth = nextPowerOfTwo(atlasWidth);
    atlasHeight = nextPowerOfTwo(atlasHeight);
    std::shared_ptr<Texture> atlas = std::make_shared<Texture>(atlasWidth, atlasHeight);

    u8 *data = NULL;
    data = new u8[atlasWidth * atlasHeight * 4];
    memset(data, 0, atlasWidth * atlasHeight * 4);

/*
    loop through sprites:
        use atlasX, atlasY to compute new U/Vs
        draw sprite to atlas texture
        delete sprite tga data
        set sprite gl texture to atlas texture
    upload atlas texture to gpu
*/

    for (std::vector<Sprite*>::iterator sprite = atlasSprites.begin(); sprite != atlasSprites.end(); sprite++) {
        f32 u1 = (f32)(*sprite)->atlasX / (f32)atlasWidth;
        f32 v1 = (f32)(*sprite)->atlasY / (f32)atlasHeight;
        f32 u2 = (f32)((*sprite)->atlasX + (*sprite)->width) / (f32)atlasWidth;
        f32 v2 = (f32)((*sprite)->atlasY + (*sprite)->height) / (f32)atlasHeight;

        //  draw sprite to atlas texture
        u8 *imageData = (*sprite)->getImage()->data;

        for (u32 y = 0; y < (*sprite)->height; y++) {
            memcpy(&data[((y + (*sprite)->atlasY) * atlasWidth + (*sprite)->atlasX)* 4],
               &imageData[((*sprite)->height - 1 - y) * (*sprite)->width * 4], (*sprite)->width * 4);
        }

        LOG("-------",0);
        LOG_EXP((*sprite)->atlasX);
        LOG_EXP((*sprite)->atlasY);

        (*sprite)->adopt(atlas, u1, v1, u2, v2);
    }
    CALL_GL(glActiveTexture(GL_TEXTURE0));
    CALL_GL(glBindTexture(GL_TEXTURE_2D, atlas->glHandle));
    CALL_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

    // saveTGA(data, atlasWidth, atlasHeight, "atlas.tga");

    atlas.reset();
    delete [] data;

    //  TODO: check if grown past MAX_TEXTURE_SIZE and split
    atlasSprites.clear();
    // GL_MAX_TEXTURE_SIZE
}

void defineSpriteFromAtlas(u32 atlasIndex, f32 u1, f32 v1, f32 u2, f32 v2, u32 spriteIndex, b8 buildCollisionMass);
    Sprite* sprite = sprites.get(spriteIndex);
    sprite->image = sprites.get(atlasIndex)->image;
    sprite->width = width;
    sprite->height = height;
    sprite->halfX = width / 2;
    sprite->halfY = height / 2;
    sprite->radius = sqrt((f32)((sprite->halfX * sprite->halfX) + (sprite->halfY * sprite->halfY)));

    if (buildCollisionMask) {
        sprite->buildCollisionMask((u32)(u * atlasWidth), (u32)(v * atlasHeight));
    }
    sprite->adopt(sprites.get(firstIndex)->texture, u1, v1, u2, v2, true);

    return 0;
}

u32 loadAtlas(std::string const& filename, u32 firstIndex, u32 width, u32 height, b8 buildCollisionMasks) {
    sprites.mapResource(firstIndex, filename);
    sprites.get(firstIndex)->uploadToGPU(true);
    
    u32 atlasWidth = sprites.get(firstIndex)->width;
    u32 atlasHeight = sprites.get(firstIndex)->height;
    
    u32 columns = atlasWidth / width;
    u32 rows = atlasHeight / height;
    u32 numSprites = rows * columns;
    
    f32 uIncrease = ((f32)width / (f32)atlasWidth) *
        ((f32)atlasWidth / (f32)sprites.get(firstIndex)->texture->width);
    f32 vIncrease = ((f32)height / (f32)atlasHeight) *
        ((f32)atlasHeight / (f32)sprites.get(firstIndex)->texture->height);
    
    u32 spriteIndex = firstIndex;
    f32 v = 0.0f;
    for (u32 y = 0; y < rows; y++) {
        f32 u = 0.0f;
        for (u32 x = 0; x < columns; x++) {
            if (spriteIndex > firstIndex) {
                sprites.mapResource(spriteIndex, ".");
            }

            f32 u1 = u;
            f32 v1 = v;
            f32 u2 = u + uIncrease;
            f32 v2 = v + vIncrease;

            defineSpriteFromAtlas(firstIndex, u1, v1, u2, v2, spriteIndex, buildCollisionMasks);

            spriteIndex++;
            u += uIncrease;
        }
        v += vIncrease;
    }
    delete sprites.get(firstIndex)->image;
    for (u32 i = firstIndex; i < spriteIndex; i++) {
        sprites.get(i)->image = NULL;
    }
    
    return numSprites;
}

}   //  namespace

