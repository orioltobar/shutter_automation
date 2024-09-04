@file:OptIn(ExperimentalResourceApi::class)

package controls

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentWidth
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
import androidx.compose.material.MaterialTheme
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.SnackbarHost
import androidx.compose.material.SnackbarHostState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Face
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.ColorFilter
import androidx.compose.ui.graphics.vector.rememberVectorPainter
import androidx.compose.ui.unit.dp
import cafe.adriel.voyager.koin.getScreenModel
import cafe.adriel.voyager.navigator.tab.Tab
import cafe.adriel.voyager.navigator.tab.TabOptions
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.getString
import org.jetbrains.compose.resources.stringResource
import org.jetbrains.compose.resources.vectorResource
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.baseline_arrow_drop_down_24
import shutters.composeapp.generated.resources.baseline_arrow_drop_up_24
import shutters.composeapp.generated.resources.controls
import shutters.composeapp.generated.resources.lower
import shutters.composeapp.generated.resources.raise

object ControlsTab: Tab {
    override val options: TabOptions
        @Composable
        get() {
            val icon = rememberVectorPainter(Icons.Default.Face)
            val title = stringResource(Res.string.controls)
            return remember {
                TabOptions(
                    index = 0u,
                    title = title,
                    icon = icon
                )
            }
        }

    @Composable
    override fun Content() {
        val model = getScreenModel<ControlsScreenModel>()
        val state by model.state.collectAsState()
        val snackbarHostState = remember { SnackbarHostState() }

        LaunchedEffect(key1 = state.errorMessage) {
            println("showing snackbar with")
            state.errorMessage?.let {
                snackbarHostState.showSnackbar(
                    message = getString(it),
                    duration = SnackbarDuration.Short
                )
                model reduce (ErrorMessageShown)
            }
        }

        MaterialTheme {
            Box(modifier = Modifier.fillMaxSize()) {
                Column(
                    modifier = Modifier.fillMaxWidth().fillMaxHeight(),
                    verticalArrangement = Arrangement.Center,
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Button(modifier = Modifier.wrapContentWidth().padding(20.dp),
                        shape = RoundedCornerShape(100),
                        onClick = { model reduce RiseButtonClicked }) {
                        Image(
                            modifier = Modifier.scale(4f)
                                .padding(vertical = 30.dp, horizontal = 20.dp),
                            imageVector = vectorResource(Res.drawable.baseline_arrow_drop_up_24),
                            colorFilter = ColorFilter.tint(Color.White),
                            contentDescription = stringResource(Res.string.lower)
                        )
                    }
                    Button(modifier = Modifier.wrapContentWidth().padding(20.dp),
                        shape = RoundedCornerShape(100),
                        onClick = { model reduce LowerButtonClicked }) {
                        Image(
                            modifier = Modifier.scale(4f)
                                .padding(vertical = 30.dp, horizontal = 20.dp),
                            imageVector = vectorResource(Res.drawable.baseline_arrow_drop_down_24),
                            colorFilter = ColorFilter.tint(Color.White),
                            contentDescription = stringResource(Res.string.raise)
                        )
                    }
                }
                SnackbarHost(
                    hostState = snackbarHostState,
                    modifier = Modifier.align(Alignment.BottomCenter)
                )
            }
        }
    }
}