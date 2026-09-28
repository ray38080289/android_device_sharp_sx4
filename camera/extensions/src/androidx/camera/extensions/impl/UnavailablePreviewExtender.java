/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package androidx.camera.extensions.impl;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.params.SessionConfiguration;
import android.util.Pair;
import android.util.Size;

import java.util.List;

/**
 * A mode this device does not offer. Replaces the AOSP sample extenders, whose
 * effects are placeholders (white balance tints), so apps do not list them.
 */
abstract class UnavailablePreviewExtender implements PreviewExtenderImpl {
    @Override
    public boolean isExtensionAvailable(String cameraId, CameraCharacteristics chars) {
        return false;
    }

    @Override
    public void init(String cameraId, CameraCharacteristics chars) {}

    @Override
    public CaptureStageImpl getCaptureStage() {
        return null;
    }

    @Override
    public ProcessorType getProcessorType() {
        return ProcessorType.PROCESSOR_TYPE_NONE;
    }

    @Override
    public ProcessorImpl getProcessor() {
        return null;
    }

    @Override
    public List<Pair<Integer, Size[]>> getSupportedResolutions() {
        return null;
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
}
