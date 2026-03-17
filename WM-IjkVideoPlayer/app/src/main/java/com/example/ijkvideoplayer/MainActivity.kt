package com.example.ijkvideoplayer

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.util.Log
import android.view.SurfaceHolder
import android.view.View
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import com.example.ijkvideoplayer.databinding.ActivityMainBinding
import com.example.ijkvideoplayer.utils.TimeUtils
import tv.danmaku.ijk.media.player.IjkMediaPlayer

class MainActivity : AppCompatActivity(), View.OnClickListener {

    private var viewWidth = 0
    private var viewHeight = 0
    private lateinit var binding: ActivityMainBinding
    private val ijkMediaPlayer by lazy { IjkMediaPlayer() }

    private val activityResultLauncher =
        registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
            val uri = result.data?.data ?: return@registerForActivityResult
            playVideo(uri)
        }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.selectBtn.setOnClickListener(this)

        ijkMediaPlayer.setOnPreparedListener { player ->
            Log.d("sqsong", "onPrepared, width: ${player.videoWidth}, height: ${player.videoHeight}, viewWidth: ${binding.surfaceView.width}, viewHeight: ${binding.surfaceView.height}")
            Log.w("sqsong", "Video  duration: ${player.duration}, ${TimeUtils.formatMillisToHMS(player.duration)}")
            val videoWidth = player.videoWidth
            val videoHeight = player.videoHeight
            if (viewWidth == 0) viewWidth = binding.surfaceView.width
            if (viewHeight == 0) viewHeight = binding.surfaceView.height
            val videoRatio = videoWidth.toFloat() / videoHeight
            val viewRatio = viewWidth.toFloat() / viewHeight
            val width: Int
            val height: Int
            if (videoRatio > viewRatio) {
                width = viewWidth
                height = (width / videoRatio).toInt()
            } else {
                height = viewHeight
                width = (height * videoRatio).toInt()
            }
            Log.d("sqsong", "width: $width, height: $height")
            binding.surfaceView.layoutParams.apply {
                this.width = width
                this.height = height
                binding.surfaceView.layoutParams = this
            }
        }
        binding.surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceCreated(holder: SurfaceHolder) {
                ijkMediaPlayer.setDisplay(holder)
                Log.d("sqsong", "surfaceCreated")
            }

            override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
                Log.d("sqsong", "surfaceChanged")
            }

            override fun surfaceDestroyed(holder: SurfaceHolder) {
                Log.d("sqsong", "surfaceDestroyed")
            }
        })
    }

    override fun onClick(v: View?) {
        when (v?.id) {
            R.id.selectBtn -> {
                activityResultLauncher.launch(Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
                    addCategory(Intent.CATEGORY_OPENABLE)
                    type = "video/*"
                })
            }
        }
    }

    private fun playVideo(videoUri: Uri) {
        try {
            ijkMediaPlayer.reset()
            ijkMediaPlayer.setDataSource(this, videoUri)
            ijkMediaPlayer.prepareAsync()
            ijkMediaPlayer.start()
        } catch (ex: Exception) {
            ex.printStackTrace()
        }
    }

    override fun onResume() {
        super.onResume()
        ijkMediaPlayer.start()
    }

    override fun onPause() {
        super.onPause()
        ijkMediaPlayer.pause()
    }

    override fun onDestroy() {
        super.onDestroy()

        ijkMediaPlayer.release()
        Log.d("sqsong", "onDestroy")
    }
}