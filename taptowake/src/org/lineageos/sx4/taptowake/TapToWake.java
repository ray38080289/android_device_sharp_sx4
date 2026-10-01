/*
 * SPDX-FileCopyrightText: The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

package org.lineageos.sx4.taptowake;

import android.app.Application;
import android.database.ContentObserver;
import android.os.Handler;
import android.os.Looper;
import android.provider.Settings;
import android.util.Log;

import java.io.FileWriter;
import java.io.IOException;

public class TapToWake extends Application {
    private static final String TAG = "Sx4TapToWake";
    private static final String NODE = "/sys/module/fih_touch/parameters/gesture_enabled";

    @Override
    public void onCreate() {
        super.onCreate();
        // ponytail: follows the system user's setting only; per-user values if multi-user matters
        getContentResolver().registerContentObserver(
                Settings.Secure.getUriFor(Settings.Secure.DOUBLE_TAP_TO_WAKE), false,
                new ContentObserver(new Handler(Looper.getMainLooper())) {
                    @Override
                    public void onChange(boolean selfChange) {
                        update();
                    }
                });
        update();
    }

    private void update() {
        boolean enabled = Settings.Secure.getInt(getContentResolver(),
                Settings.Secure.DOUBLE_TAP_TO_WAKE, 0) != 0;
        try (FileWriter writer = new FileWriter(NODE)) {
            writer.write(enabled ? "1" : "0");
        } catch (IOException e) {
            Log.e(TAG, "Failed to write " + NODE, e);
        }
    }
}
