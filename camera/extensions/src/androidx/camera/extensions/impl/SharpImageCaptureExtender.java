/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package androidx.camera.extensions.impl;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.params.SessionConfiguration;
import android.util.Pair;
import android.util.Range;
import android.util.Size;

import java.util.ArrayList;
import java.util.List;

/** Capture for a {@link SharpFeature}: the HAL processes the frames, so no CaptureProcessor. */
abstract class SharpImageCaptureExtender implements ImageCaptureExtenderImpl {
    private static final CaptureRequest.Key[] REQUEST_KEYS = {
        CaptureRequest.CONTROL_AF_MODE,
        CaptureRequest.CONTROL_AF_REGIONS,
        CaptureRequest.CONTROL_AF_TRIGGER,
        CaptureRequest.CONTROL_ZOOM_RATIO,
    };
    private static final CaptureResult.Key[] RESULT_KEYS = {
        CaptureResult.CONTROL_AE_STATE,
        CaptureResult.CONTROL_AF_STATE,
        CaptureResult.CONTROL_AF_MODE,
        CaptureResult.CONTROL_AF_REGIONS,
        CaptureResult.CONTROL_AF_TRIGGER,
        CaptureResult.CONTROL_AWB_STATE,
        CaptureResult.SENSOR_SENSITIVITY,
        CaptureResult.SENSOR_EXPOSURE_TIME,
        CaptureResult.SENSOR_FRAME_DURATION,
        CaptureResult.LENS_FOCUS_DISTANCE,
        CaptureResult.CONTROL_ZOOM_RATIO,
        CaptureResult.SENSOR_TIMESTAMP,
    };

    private final SharpFeature mFeature;
    private CameraCharacteristics mChars;

    SharpImageCaptureExtender(SharpFeature feature) {
        mFeature = feature;
    }

    @Override
    public boolean isExtensionAvailable(String cameraId, CameraCharacteristics chars) {
        return SharpFeature.isAvailable(cameraId);
    }

    @Override
    public void init(String cameraId, CameraCharacteristics chars) {
        mChars = chars;
    }

    @Override
    public CaptureProcessorImpl getCaptureProcessor() {
        return null;
    }

    @Override
    public List<CaptureStageImpl> getCaptureStages() {
        List<CaptureStageImpl> stages = new ArrayList<>();
        stages.add(mFeature.stage());
        return stages;
    }

    @Override
    public int getMaxCaptureStage() {
        return 1;
    }

    @Override
    public List<Pair<Integer, Size[]>> getSupportedResolutions() {
        return null;
    }

    @Override
    public List<Pair<Integer, Size[]>> getSupportedPostviewResolutions(Size captureSize) {
        return new ArrayList<>();
    }

    @Override
    public Range<Long> getEstimatedCaptureLatencyRange(Size captureOutputSize) {
        return null;
    }

    @Override
    public List<CaptureRequest.Key> getAvailableCaptureRequestKeys() {
        List<CaptureRequest.Key> keys = new ArrayList<>();
        if (mChars == null) return keys;
        List<CaptureRequest.Key<?>> supported = mChars.getAvailableCaptureRequestKeys();
        for (CaptureRequest.Key k : REQUEST_KEYS) {
            if (supported.contains(k)) keys.add(k);
        }
        return keys;
    }

    @Override
    public List<CaptureResult.Key> getAvailableCaptureResultKeys() {
        List<CaptureResult.Key> keys = new ArrayList<>();
        if (mChars == null) return keys;
        List<CaptureResult.Key<?>> supported = mChars.getAvailableCaptureResultKeys();
        for (CaptureResult.Key k : RESULT_KEYS) {
            if (supported.contains(k)) keys.add(k);
        }
        return keys;
    }

    @Override
    public void onInit(String cameraId, CameraCharacteristics chars, Context context) {}

    @Override
    public void onDeInit() {}

    @Override
    public CaptureStageImpl onPresetSession() {
        return mFeature.stage();
    }

    @Override
    public CaptureStageImpl onEnableSession() {
        return mFeature.stage();
    }

    @Override
    public CaptureStageImpl onDisableSession() {
        return null;
    }

    @Override
    public int onSessionType() {
        return SessionConfiguration.SESSION_REGULAR;
    }

    @Override
    public boolean isCaptureProcessProgressAvailable() {
        return false;
    }

    @Override
    public Pair<Long, Long> getRealtimeCaptureLatency() {
        return null;
    }

    @Override
    public boolean isPostviewAvailable() {
        return false;
    }
}
