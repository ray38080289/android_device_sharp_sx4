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

/** Image capture side of {@link UnavailablePreviewExtender}. */
abstract class UnavailableImageCaptureExtender implements ImageCaptureExtenderImpl {
    @Override
    public boolean isExtensionAvailable(String cameraId, CameraCharacteristics chars) {
        return false;
    }

    @Override
    public void init(String cameraId, CameraCharacteristics chars) {}

    @Override
    public CaptureProcessorImpl getCaptureProcessor() {
        return null;
    }

    @Override
    public List<CaptureStageImpl> getCaptureStages() {
        return new ArrayList<>();
    }

    @Override
    public int getMaxCaptureStage() {
        return 0;
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
        return new ArrayList<>();
    }

    @Override
    public List<CaptureResult.Key> getAvailableCaptureResultKeys() {
        return new ArrayList<>();
    }

    @Override
    public void onInit(String cameraId, CameraCharacteristics chars, Context context) {}

    @Override
    public void onDeInit() {}

    @Override
    public CaptureStageImpl onPresetSession() {
        return null;
    }

    @Override
    public CaptureStageImpl onEnableSession() {
        return null;
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
