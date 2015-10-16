#pragma once

#include "../image/image.h"
#include "../image/color_range.h"
#include "../maniac/rac.h"
#include "../flif_config.h"
#include "../common.h"


template <typename IO>
class Transform {
protected:

public:
    virtual ~Transform() {};

    // On encode: init, process, save, meta, data, <processing>
    // On decode: init,          load, meta,       <processing>, invData           ( + optional configure anywhere)

    bool virtual init(const ColorRanges *) { return true; }
    void virtual configure(const int) { }
    void virtual load(const ColorRanges *, RacIn<IO> &) {};
#ifdef HAS_ENCODER
    bool virtual process(const ColorRanges *, const Images &) { return true; };
    void virtual save(const ColorRanges *, RacOut<IO> &) const {};
    void virtual data(Images&) const {}
#endif
    const ColorRanges virtual *meta(Images&, const ColorRanges *srcRanges) { return new DupColorRanges(srcRanges); }
    void virtual invData(Images&) const {}

};
