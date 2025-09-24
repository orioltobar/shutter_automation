@file:OptIn(ExperimentalResourceApi::class)

package controls

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentWidth
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.Button
import androidx.compose.material.MaterialTheme
import androidx.compose.material.RadioButton
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.SnackbarHost
import androidx.compose.material.SnackbarHostState
import androidx.compose.material.Text
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
import model.BlindType
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.getString
import org.jetbrains.compose.resources.stringResource
import org.jetbrains.compose.resources.vectorResource
import org.jetbrains.compose.ui.tooling.preview.Preview
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.all_blinds
import shutters.composeapp.generated.resources.baseline_arrow_drop_down_24
import shutters.composeapp.generated.resources.baseline_arrow_drop_up_24
import shutters.composeapp.generated.resources.center_blinds
import shutters.composeapp.generated.resources.controls
import shutters.composeapp.generated.resources.left_blinds
import shutters.composeapp.generated.resources.lower
import shutters.composeapp.generated.resources.raise
import shutters.composeapp.generated.resources.right_blinds
import shutters.composeapp.generated.resources.stop

object ControlsTab : Tab {
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
            ControlsTabScreen(
                selectedButtons = state.selectedBlind,
                snackbarHostState = snackbarHostState,
                onRiseButtonClicked = { model reduce RiseButtonClicked },
                onLowerButtonClicked = { model reduce LowerButtonClicked },
                onStopButtonClicked = { model reduce StopButtonClicked },
                onRadioButtonClick = { type -> model reduce BlindSelectorClicked(type) }
            )
        }
    }
}

@Composable
private fun ControlsTabScreen(
    selectedButtons: BlindType,
    snackbarHostState: SnackbarHostState,
    onRiseButtonClicked: () -> Unit,
    onLowerButtonClicked: () -> Unit,
    onStopButtonClicked: () -> Unit,
    onRadioButtonClick: (BlindType) -> Unit
) {
    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(Color.White)
    ) {
        Column(
            modifier = Modifier.fillMaxWidth().fillMaxHeight(),
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            BlindSelectorComponent(
                selectedButtons = selectedButtons,
                onClick = onRadioButtonClick
            )
            Button(
                modifier = Modifier.wrapContentWidth().padding(20.dp),
                shape = RoundedCornerShape(100),
                onClick = { onRiseButtonClicked() }) {
                Image(
                    modifier = Modifier.scale(4f)
                        .padding(vertical = 30.dp, horizontal = 20.dp),
                    imageVector = vectorResource(Res.drawable.baseline_arrow_drop_up_24),
                    colorFilter = ColorFilter.tint(Color.White),
                    contentDescription = stringResource(Res.string.lower)
                )
            }
            Button(
                modifier = Modifier.wrapContentWidth().padding(20.dp),
                shape = RoundedCornerShape(100),
                onClick = { onStopButtonClicked() }) {
                Text(
                    modifier = Modifier
                        .padding(vertical = 30.dp, horizontal = 20.dp),
                    text = stringResource(Res.string.stop),
                )
            }
            Button(
                modifier = Modifier.wrapContentWidth().padding(20.dp),
                shape = RoundedCornerShape(100),
                onClick = { onLowerButtonClicked() }) {
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

@Composable
private fun BlindSelectorComponent(
    onClick: (BlindType) -> Unit,
    selectedButtons: BlindType = BlindType.ALL
) {
    Row {
        RadioButtonComponent(
            text = stringResource(Res.string.all_blinds),
            isSelected = selectedButtons == BlindType.ALL,
            onClick = { onClick(BlindType.ALL) }
        )
        RadioButtonComponent(
            text = stringResource(Res.string.left_blinds),
            isSelected = selectedButtons == BlindType.LEFT,
            onClick = { onClick(BlindType.LEFT) }
        )
        RadioButtonComponent(
            text = stringResource(Res.string.center_blinds),
            isSelected = selectedButtons == BlindType.CENTER,
            onClick = { onClick(BlindType.CENTER) }
        )
        RadioButtonComponent(
            text = stringResource(Res.string.right_blinds),
            isSelected = selectedButtons == BlindType.RIGHT,
            onClick = { onClick(BlindType.RIGHT) }
        )
    }
}

@Composable
private fun RadioButtonComponent(
    text: String,
    isSelected: Boolean = false,
    onClick: () -> Unit = {}
) {
    Row(
        verticalAlignment = Alignment.CenterVertically
    ) {
        RadioButton(
            selected = isSelected,
            onClick = { onClick() }
        )
        Text(
            text = text,
        )
    }
}

@Preview
@Composable
private fun ControlsTabPreview() {
    MaterialTheme {
        ControlsTabScreen(
            selectedButtons = BlindType.ALL,
            snackbarHostState = remember { SnackbarHostState() },
            onRiseButtonClicked = {},
            onLowerButtonClicked = {},
            onStopButtonClicked = {},
            onRadioButtonClick = {}
        )
    }
}
