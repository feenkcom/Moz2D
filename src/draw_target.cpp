/*
 * draw_target.cpp
 *
 *  Created on: May 29, 2016
 *      Author: aliaksei
 */

#include "draw_target.h"

#include "2d/Filters.h"
#include "gfxPlatform.h"
#include "2d/Logging.h"
#include "cairo/cairo/src/cairo-pdf.h"
#include "cairo/cairo/src/cairo-svg.h"

#include <stack> // std::stack

using namespace mozilla::gfx;

/* --------------------------------------------------- */
/* ----------------- C R E A T I O N ----------------- */
/* --------------------------------------------------- */

/* --------------- D R A W -- T A R G E T ------------ */

DrawTarget* moz2d_draw_target_create (int32_t width, int32_t height, SurfaceFormat aFormat) {
	return gfxPlatform::GetPlatform()->CreateOffscreenCanvasDrawTarget(IntSize(width, height), aFormat).take();
}

DrawTarget* moz2d_draw_target_create_type (BackendType aBackend, int32_t width, int32_t height, SurfaceFormat aFormat) {
	return Factory::CreateDrawTarget(aBackend, IntSize(width, height), aFormat).take();
}

DrawTarget* moz2d_draw_target_create_for_data_type (BackendType aBackend, unsigned char* aData, int32_t width, int32_t height, int32_t aStride, SurfaceFormat aFormat, bool aUninitialized) {
	return Factory::CreateDrawTargetForData(aBackend, aData, IntSize(width, height), aStride, aFormat, aUninitialized).take();
}

DrawTarget* moz2d_draw_target_create_for_data (unsigned char* aData, int32_t width, int32_t height, int32_t aStride, SurfaceFormat aFormat) {
	return gfxPlatform::GetPlatform()->CreateDrawTargetForData(aData, IntSize(width, height), aStride, aFormat).take();
}

DrawTarget* moz2d_draw_target_create_for_cairo_surface(cairo_surface_t* aSurface, int32_t width, int32_t height, SurfaceFormat aFormat) {
	return Factory::CreateDrawTargetForCairoSurface(aSurface, IntSize(width, height), &aFormat).take();
}

DrawTarget* moz2d_draw_target_create_similar(DrawTarget* drawTarget, int32_t width, int32_t height) {
	 return drawTarget->CreateSimilarDrawTarget(IntSize(width, height), drawTarget->GetFormat()).take();
}

cairo_surface_t* moz2d_draw_target_create_cairo_surface_for_svg(const char *filename, double width, double height) {
    return cairo_svg_surface_create(filename, width, height);
}

cairo_surface_t* moz2d_draw_target_create_cairo_surface_for_pdf(const char *filename, double width, double height) {
    return cairo_pdf_surface_create(filename, width, height);
}

void moz2d_draw_target_cairo_surface_destroy(cairo_surface_t* surface) {
    cairo_surface_destroy(surface);
}

/* --------------- S O U R C E -- S U R F A C E ------------ */
SourceSurface* moz2d_draw_target_create_surface_for_data (
		DrawTarget* drawTarget,
		unsigned char *aData,
        int32_t width,
        int32_t height,
        int32_t aStride,
        SurfaceFormat aFormat) {

	return drawTarget->CreateSourceSurfaceFromData(aData, IntSize(width, height), aStride, aFormat).take();
}

SourceSurface* moz2d_draw_target_create_surface_for_data_form (
		DrawTarget* drawTarget,
		unsigned char *aData,
        int32_t width,
        int32_t height,
        int32_t aStride,
        SurfaceFormat aFormat) {

	int32_t size = width * height;
	// every pixel takes 4 uchars, so we need to multiply size by 4

	unsigned char *data = new unsigned char[ 4 * size ];
	for (int32_t i = 0; i < size; i++) {
		int32_t index = i * 4;
		unsigned char alpha = aData[index + 3];

		unsigned char red = (unsigned char)(aData[index] / 255.0f * alpha);
		unsigned char green = (unsigned char)(aData[index + 1] / 255.0f * alpha);
		unsigned char blue = (unsigned char)(aData[index + 2] / 255.0f * alpha);

		data[index] = red;
		data[index + 1] = green;
		data[index + 2] = blue;
		data[index + 3] = alpha;
	}

	SourceSurface* surface = moz2d_draw_target_create_surface_for_data(drawTarget, data, width, height, aStride, aFormat);
	delete[] data;
	return surface;
}

SourceSurface* moz2d_draw_target_create_surface_for_data_b8g8r8 (
        DrawTarget* drawTarget,
        unsigned char *aData,
        int32_t width,
        int32_t height,
        int32_t aStride) {

    int32_t size = width * height;
    // every pixel takes 4 uchars, so we need to multiply size by 4

    unsigned char *data = new unsigned char[ 4 * size ];

    size_t s, d;
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            s = y * aStride + (x * 3);
            d = y * width * 4 + (x * 4);

            data[d] = aData[s];
            data[d + 1] = aData[s + 1];
            data[d + 2] = aData[s + 2];
            data[d + 3] = 255;
        }
    }

    SourceSurface* surface = moz2d_draw_target_create_surface_for_data(drawTarget, data, width, height, width * 4, SurfaceFormat::B8G8R8X8);
    delete[] data;
    return surface;
}

SourceSurface* moz2d_draw_target_create_surface_from_native (
		DrawTarget* drawTarget,
		NativeSurfaceType aType,
		SurfaceFormat aFormat,
		int32_t width,
		int32_t height,
		void *aSurface) {

	NativeSurface nativeSurface = {
			aType, aFormat, IntSize(width, height), aSurface
	};

	return drawTarget->CreateSourceSurfaceFromNativeSurface(nativeSurface).take();
}

/* --------------------------------------------------- */
/* ------------------- T E S T I N G ----------------- */
/* --------------------------------------------------- */

bool moz2d_draw_target_is_valid(DrawTarget *drawTarget) {
	return drawTarget->IsValid();
}

bool moz2d_draw_target_is_recording(DrawTarget *drawTarget) {
	return drawTarget->IsRecording();
}

bool moz2d_draw_target_get_permit_subpixel_aa(DrawTarget *drawTarget) {
	return drawTarget->GetPermitSubpixelAA();
}

/* --------------------------------------------------- */
/* ----------------- A C C E S S I N G --------------- */
/* --------------------------------------------------- */

DrawTargetType moz2d_draw_target_get_type (DrawTarget *drawTarget) {
	return drawTarget->GetType();
}

BackendType moz2d_draw_target_get_backend_type (DrawTarget *drawTarget) {
	return drawTarget->GetBackendType();
}

SurfaceFormat moz2d_draw_target_get_surface_format (DrawTarget *drawTarget) {
	return drawTarget->GetFormat();
}

void moz2d_draw_target_get_size(DrawTarget *drawTarget, IntSize* aSize) {
	IntSize size = drawTarget->GetSize();
	aSize->width = size.width;
	aSize->height = size.height;
}

SourceSurface* moz2d_draw_target_snapshot(DrawTarget *drawTarget) {
	return drawTarget->Snapshot().take();
}

void moz2d_draw_target_flush(DrawTarget *drawTarget) {
	drawTarget->Flush();
}

void moz2d_draw_target_set_permit_subpixel_aa(DrawTarget *drawTarget, bool aPermitSubpixelAA) {
	drawTarget->SetPermitSubpixelAA(aPermitSubpixelAA);
}

uint32_t * moz2d_draw_target_get_data(DrawTarget* drawTarget) {
	RefPtr<SourceSurface> snapshot = drawTarget->Snapshot();
	RefPtr<DataSourceSurface> mDataSnapshot = snapshot->GetDataSurface();
	return (uint32_t *) mDataSnapshot->GetData();
}

int32_t moz2d_draw_target_get_stride(DrawTarget* drawTarget) {
	RefPtr<SourceSurface> snapshot = drawTarget->Snapshot();
	RefPtr<DataSourceSurface> mDataSnapshot = snapshot->GetDataSurface();
	return mDataSnapshot->Stride();
}

void moz2d_source_surface_get_size(SourceSurface *aSourceSurface, IntSize* aSize) {
	IntSize size = aSourceSurface->GetSize();
	aSize->width = size.width;
	aSize->height = size.height;
}

SurfaceFormat moz2d_source_surface_get_format(SourceSurface *aSourceSurface) {
	return aSourceSurface->GetFormat();
}

SurfaceType moz2d_source_surface_get_type(SourceSurface *aSourceSurface) {
	return aSourceSurface->GetType();
}

LIBRARY_API void moz2d_draw_target_as_form(DrawTarget* drawTarget, uint32_t *aData) {
	RefPtr<SourceSurface> snapshot = drawTarget->Snapshot();
	RefPtr<DataSourceSurface> dataSurface = snapshot->GetDataSurface();
	DataSourceSurface::MappedSurface mappingSurface;
	if (!dataSurface->Map(DataSourceSurface::MapType::READ, &mappingSurface)) {
		gfxCriticalError() << "moz2d_draw_target_as_form failed to map surface";
	}

	uint32_t* surfaceData = (uint32_t*) mappingSurface.mData;
	// since data is originally uint8_t
	int32_t surfaceStride = mappingSurface.mStride / 4;
	IntSize surfaceSize = dataSurface->GetSize();

	for (int32_t y = 0; y < surfaceSize.height; y++) {
		for (int32_t x = 0; x < surfaceSize.width; x++) {
			uint32_t pixel = surfaceData[ y * surfaceStride + x ];
			aData[ y * surfaceSize.width + x ] = pixel;
		}
	}

	dataSurface->Unmap();
}

/* --------------------------------------------------- */
/* ------------------- C L I P P I N G --------------- */
/* --------------------------------------------------- */
static UserDataKey sClippingBounds;

void moz2d_draw_target_pop_clip(DrawTarget* drawTarget) {
	drawTarget->PopClip();

	std::stack<Rect> *stack = static_cast<std::stack<Rect>*>(drawTarget->GetUserData(&sClippingBounds));
	if (!stack) {
		return;
	}
	if (stack->empty()) {
		return;
	}

	stack->pop();
}

/*
 * Is called when DrawTarget gets deleted (refCount becomes zero)
 */
void ClipStackDestroyed(void *aData)
{
    delete static_cast<std::stack<Rect>*>(aData);
}
/**
 * Clip is in user space
 */
void push_clip(DrawTarget* drawTarget, Rect clip) {
	Rect deviceClip = drawTarget->GetTransform().TransformBounds(clip);

	std::stack<Rect> *stack = static_cast<std::stack<Rect>*>(drawTarget->GetUserData(&sClippingBounds));
	if (!stack) {
		stack = new std::stack<Rect>();
		drawTarget->AddUserData(&sClippingBounds, stack, ClipStackDestroyed);
	}
	Rect newClip = deviceClip;
	if (!stack->empty()) {
		newClip = stack->top().Intersect(deviceClip);
	}
	stack->push(newClip);
}

void moz2d_draw_target_push_clip_rectangle (DrawTarget* drawTarget, Float x, Float y, Float width, Float height) {
	Rect clip = Rect(x,y, width, height);

	drawTarget->PushClipRect(clip);

	push_clip(drawTarget, clip);
}

void moz2d_draw_target_push_clip_path (DrawTarget* drawTarget, Path* aPath) {
	drawTarget->PushClip(aPath);

	push_clip(drawTarget, aPath->GetBounds(Matrix()));
}

/**
 * Return clip in device space
 */
Rect get_clip(DrawTarget* drawTarget) {
	std::stack<Rect> *stack = static_cast<std::stack<Rect>*>(drawTarget->GetUserData(&sClippingBounds));
	if (!stack) {
		IntSize extent = drawTarget->GetSize();
		return Rect(0,0, extent.width, extent.height);
	}

	if (stack->empty()) {
		IntSize extent = drawTarget->GetSize();
		return Rect(0,0, extent.width, extent.height);
	}

	return stack->top();
}

void moz2d_draw_target_clipping_bounds_global(DrawTarget* drawTarget, Rect* rectangle) {
	Rect clip = get_clip(drawTarget);

	rectangle->x = clip.x;
	rectangle->y = clip.y;
	rectangle->width = clip.width;
	rectangle->height = clip.height;
}

void moz2d_draw_target_clipping_bounds_local(DrawTarget* drawTarget, Rect* rectangle) {
	Rect clip = drawTarget->GetTransform().Inverse().TransformBounds(get_clip(drawTarget));

	rectangle->x = clip.x;
	rectangle->y = clip.y;
	rectangle->width = clip.width;
	rectangle->height = clip.height;
}

bool moz2d_draw_target_clipping_is_in_local(DrawTarget* drawTarget, Float x, Float y, Float width, Float height) {
	Rect clip = drawTarget->GetTransform().Inverse().TransformBounds(get_clip(drawTarget));
	return clip.Intersects(Rect(x,y,width,height));
}

bool moz2d_draw_target_clipping_is_in_global(DrawTarget* drawTarget, Float x, Float y, Float width, Float height) {
	Rect clip = get_clip(drawTarget);
	return clip.Intersects(Rect(x,y,width,height));
}

/* --------------------------------------------------- */
/* ----------------------- LAYERS -------------------- */
/* --------------------------------------------------- */

LIBRARY_API void moz2d_draw_target_push_layer(DrawTarget* drawTarget, bool aOpaque, Float aOpacity, SourceSurface* aMask, Float a11, Float a12, Float a21, Float a22, Float a31, Float a32, int32_t x, int32_t y, int32_t width, int32_t height, bool aCopyBackground) {
    drawTarget->PushLayer(aOpaque, aOpacity, aMask, Matrix(a11, a12, a21, a22, a31, a32), IntRect(x,y,width,height), aCopyBackground);
}

void moz2d_draw_target_pop_layer(DrawTarget* drawTarget) {
	drawTarget->PopLayer();
}

/* --------------------------------------------------- */
/* -------------------- D R A W I N G ---------------- */
/* --------------------------------------------------- */

/*
 * Fill
 */
void moz2d_draw_target_fill_rect(DrawTarget* drawTarget, Rect* rect, Pattern* pattern, DrawOptions* drawOptions) {
	drawTarget->FillRect(*rect, *pattern, *drawOptions);
}

void moz2d_draw_target_fill_rect_color(DrawTarget* drawTarget, Float x, Float y, Float width, Float height, Float r, Float g, Float b, Float a, DrawOptions* drawOptions) {
	drawTarget->FillRect(Rect(x,y,width,height), ColorPattern(Color(r,g,b,a)), *drawOptions);
}

void moz2d_draw_target_fill_rectangle_form (DrawTarget* drawTarget, Float x, Float y, Float width, Float height, DrawOptions* drawOptions, unsigned char *aFormData, int32_t aFormWidth, int32_t aFormHeight, int32_t aStride) {
    SourceSurface* aSourceSurface = moz2d_draw_target_create_surface_for_data_form(drawTarget, aFormData, aFormWidth, aFormHeight, aStride, drawTarget->GetFormat());
	drawTarget->DrawSurface(aSourceSurface, Rect(x,y,width,height), Rect(0,0,aFormWidth,aFormHeight), DrawSurfaceOptions(), *drawOptions);
    aSourceSurface->Release();
}

void moz2d_draw_target_fill_path(DrawTarget* drawTarget, Path* path, Pattern* pattern, DrawOptions* drawOptions) {
	drawTarget->Fill(path, *pattern, *drawOptions);
}

void moz2d_draw_target_fill_path_color (DrawTarget* drawTarget, Path* path, Float r, Float g, Float b, Float a, DrawOptions* drawOptions) {
	drawTarget->Fill(path, ColorPattern(Color(r,g,b,a)), *drawOptions);
}

void moz2d_draw_target_fill_path_form (DrawTarget* drawTarget, Path* path, DrawOptions* drawOptions, unsigned char *aFormData, int32_t aFormWidth, int32_t aFormHeight, int32_t aStride) {
    SourceSurface* aSourceSurface = moz2d_draw_target_create_surface_for_data_form(drawTarget, aFormData, aFormWidth, aFormHeight, aStride, drawTarget->GetFormat());
    drawTarget->Fill(path, SurfacePattern(aSourceSurface, ExtendMode::CLAMP), *drawOptions);
    aSourceSurface->Release();
}

/*
 * Stroke
 */
void moz2d_draw_target_stroke_rect(DrawTarget* drawTarget, Rect* rect, Pattern* pattern, StrokeOptions* strokeOptions, DrawOptions* drawOptions) {
	drawTarget->StrokeRect(*rect, *pattern, *strokeOptions, *drawOptions);
}

void moz2d_draw_target_stroke_rect_color(DrawTarget* drawTarget, Float x, Float y, Float width, Float height, Float r, Float g, Float b, Float a, StrokeOptions* strokeOptions, DrawOptions* drawOptions) {
	drawTarget->StrokeRect(Rect(x,y,width,height), ColorPattern(Color(r,g,b,a)), *strokeOptions, *drawOptions);
}

void moz2d_draw_target_stroke_path(DrawTarget* drawTarget, Path* path, Pattern* pattern, StrokeOptions* strokeOptions, DrawOptions* drawOptions) {
	drawTarget->Stroke(path, *pattern, *strokeOptions, *drawOptions);
}

void moz2d_draw_target_stroke_path_color(DrawTarget* drawTarget, Path* path, Float r, Float g, Float b, Float a, StrokeOptions* strokeOptions, DrawOptions* drawOptions) {
	drawTarget->Stroke(path, ColorPattern(Color(r,g,b,a)), *strokeOptions, *drawOptions);
}

void moz2d_draw_target_stroke_line(DrawTarget* drawTarget, Float fromX, Float fromY, Float toX, Float toY, Pattern* pattern, StrokeOptions* strokeOptions, DrawOptions* drawOptions) {
	drawTarget->StrokeLine(Point(fromX, fromY), Point(toX, toY), *pattern, *strokeOptions, *drawOptions);
}

void moz2d_draw_target_stroke_line_color(DrawTarget* drawTarget, Float fromX, Float fromY, Float toX, Float toY, Float r, Float g, Float b, Float a, StrokeOptions* strokeOptions, DrawOptions* drawOptions) {
	drawTarget->StrokeLine(Point(fromX, fromY), Point(toX, toY), ColorPattern(Color(r,g,b,a)), *strokeOptions, *drawOptions);
}

/*
 * Mask
 */
void moz2d_draw_target_mask_pattern(DrawTarget* drawTarget, Pattern* aSource, Pattern* aMask, DrawOptions* aOptions) {
	drawTarget->Mask(*aSource, *aMask, *aOptions);
}

void moz2d_draw_target_mask_surface(DrawTarget* drawTarget, Pattern* aSource, SourceSurface *aMask, Float offsetX, Float offsetY, DrawOptions* aOptions) {
	drawTarget->MaskSurface(*aSource, aMask, Point(offsetX, offsetY), *aOptions);
}

/*
 * Filter
 */
void moz2d_draw_target_draw_filter(DrawTarget* drawTarget, FilterNode* aFilter, Rect* sourceRect, Float destX, Float destY, DrawOptions* drawOptions) {
	drawTarget->DrawFilter(aFilter, *sourceRect, Point(destX, destY), *drawOptions);
}

/*
 * Path
 */
PathBuilder* moz2d_draw_target_create_path_builder(DrawTarget* drawTarget, FillRule aFillRule) {
	return drawTarget->CreatePathBuilder(aFillRule).take();
}

/*
 * 3D Transformed
 */
bool moz2d_draw_target_draw_3d_transformed(DrawTarget* drawTarget, SourceSurface* aSurface, Float* rawMatrix) {
    Matrix4x4 matrix = Matrix4x4(
            rawMatrix[0], rawMatrix[1], rawMatrix[2], rawMatrix[3],
            rawMatrix[4], rawMatrix[5], rawMatrix[6], rawMatrix[7],
            rawMatrix[8], rawMatrix[9], rawMatrix[10], rawMatrix[11],
            rawMatrix[12], rawMatrix[13], rawMatrix[14], rawMatrix[15]);

    return drawTarget->Draw3DTransformedSurface(aSurface, matrix);
}

/* --------------------------------------------------- */
/* ----------- T R A N S F O R M A T I O N ----------- */
/* --------------------------------------------------- */

void moz2d_draw_target_transform_get(DrawTarget* drawTarget, Float* array) {
	Matrix matrix = drawTarget->GetTransform();
	array[0] = matrix._11;
	array[1] = matrix._12;
	array[2] = matrix._21;
	array[3] = matrix._22;
	array[4] = matrix._31;
	array[5] = matrix._32;
}

void moz2d_draw_target_transform_set(DrawTarget* drawTarget, Float* rawMatrix) {
	drawTarget->SetTransform(Matrix(rawMatrix[0],rawMatrix[1],rawMatrix[2],rawMatrix[3],rawMatrix[4],rawMatrix[5]));
}

void moz2d_draw_target_transform_concatenate(DrawTarget* drawTarget, Float a11, Float a12, Float a21, Float a22, Float a31, Float a32) {
	drawTarget->ConcatTransform(Matrix(a11, a12, a21, a22, a31, a32));
}

static UserDataKey sTransformKey;
/*
 * Is called when DrawTarget gets deleted (refCount becomes zero)
 */
void TransformStackDestroyed(void *aData)
{
    delete static_cast<std::stack<Rect>*>(aData);
}
void moz2d_draw_target_transform_push(DrawTarget* drawTarget) {
	Matrix transform = drawTarget->GetTransform();

	std::stack<Matrix> *stack = static_cast<std::stack<Matrix>*>(drawTarget->GetUserData(&sTransformKey));
	if (!stack) {
		stack = new std::stack<Matrix>();
		drawTarget->AddUserData(&sTransformKey, stack, TransformStackDestroyed);
	}

	stack->push(transform);
}

void moz2d_draw_target_transform_pop(DrawTarget* drawTarget) {
	std::stack<Matrix> *stack = static_cast<std::stack<Matrix>*>(drawTarget->GetUserData(&sTransformKey));
	if (!stack) {
		return;
	}

	if (stack->empty()) {
		return;
	}

	Matrix transform = stack->top();
	stack->pop();

	drawTarget->SetTransform(transform);
}
