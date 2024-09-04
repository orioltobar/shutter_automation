package automations

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ColumnScope
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.wrapContentHeight
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.ClickableText
import androidx.compose.material.Button
import androidx.compose.material.LinearProgressIndicator
import androidx.compose.material.MaterialTheme
import androidx.compose.material.SnackbarDuration
import androidx.compose.material.SnackbarHost
import androidx.compose.material.SnackbarHostState
import androidx.compose.material.Switch
import androidx.compose.material.Text
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.rememberVectorPainter
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.unit.dp
import cafe.adriel.voyager.koin.getScreenModel
import cafe.adriel.voyager.navigator.tab.Tab
import cafe.adriel.voyager.navigator.tab.TabOptions
import kotlinx.datetime.LocalTime
import model.AutomationType
import network.chaintech.ui.timepicker.WheelTimePickerDialog
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.getString
import org.jetbrains.compose.resources.painterResource
import org.jetbrains.compose.resources.stringResource
import org.jetbrains.compose.resources.vectorResource
import org.jetbrains.compose.ui.tooling.preview.Preview
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.automations
import shutters.composeapp.generated.resources.baseline_arrow_drop_down_24
import shutters.composeapp.generated.resources.baseline_arrow_drop_up_24
import shutters.composeapp.generated.resources.baseline_block_24
import shutters.composeapp.generated.resources.error_no_network
import shutters.composeapp.generated.resources.retry
import shutters.composeapp.generated.resources.type

@OptIn(ExperimentalResourceApi::class)
object AutomationsTab: Tab {
    override val options: TabOptions
        @Composable
        get() {
            val icon = rememberVectorPainter(Icons.Default.Home)
            val title = stringResource(Res.string.automations)
            return remember {
                TabOptions(
                    index = 0u,
                    title = title,
                    icon = icon
                )
            }
        }

    @Preview
    @Composable
    override fun Content() {
        val model = getScreenModel<AutomationsScreenModel>()
        val state by model.state.collectAsState()
        val snackbarHostState = remember { SnackbarHostState() }

        LaunchedEffect(key1 = true) {
            model reduce(ViewCreated)
        }

        LaunchedEffect(key1 = state.errorMessage) {
            state.errorMessage?.let {
                snackbarHostState.showSnackbar(
                    message = getString(it.message, *it.args),
                    duration = SnackbarDuration.Short
                )
                model reduce (ErrorMessageShown)
            }
        }

        MaterialTheme {
            Box(modifier = Modifier.fillMaxSize()) {
                Column {
                    if (state.showNetworkError) {
                        NoNetworkError { model reduce (it) }
                    } else {
                        if (state.isLoading) {
                            Loader()
                        } else {
                            Spacer(Modifier.height(4.dp))
                        }

                        for ((index, automation) in state.automations.withIndex()) {
                            AutomationView(index, automation) { model reduce it }
                        }
                    }
                }
                SnackbarHost(
                    hostState = snackbarHostState,
                    modifier = Modifier.align(Alignment.BottomCenter)
                )
            }
        }
    }

    @Composable
    fun AutomationView(index: Int, automation: AutomationVM, reduce: (AutomationsAction) -> Unit) {
        AutomationBox {
            Row(verticalAlignment = Alignment.CenterVertically) {
                ClickableText(
                    modifier = Modifier.weight(2f),
                    style = MaterialTheme.typography.h3,
                    text = AnnotatedString("${automation.hour}:${automation.minute}")
                ) {
                    reduce(AutomationTimeClicked(index))
                }
                Image(
                    modifier = Modifier.weight(1f).scale(2f),
                    painter = if (automation.type == AutomationType.Rise) {
                        painterResource(Res.drawable.baseline_arrow_drop_up_24)
                    } else {
                        painterResource(Res.drawable.baseline_arrow_drop_down_24)
                    }, contentDescription = stringResource(Res.string.type)
                )
                Switch(
                    modifier = Modifier.weight(1f).scale(1.5f),
                    checked = automation.isActive,
                    onCheckedChange = { reduce(AutomationActiveToggle(index)) })
            }
        }
        Spacer(modifier = Modifier.height(10.dp))

        if (automation.isSetting) {
            TimePickerDialog(automation, reduce)
        }
    }

    @Composable
    fun TimePickerDialog(automation: AutomationVM, reduce: (AutomationsAction) -> Unit) {
        WheelTimePickerDialog(height = 150.dp,
            showTimePicker = automation.isSetting,
            startTime = LocalTime(automation.hour, automation.minute),
            onDoneClick = {
                reduce(TimeSet(it))
            }) {
                reduce(TimeSetDismissed)
            }
    }

    @Composable
    fun NoNetworkError(reduce: (AutomationsAction) -> Unit) {
        Box(modifier = Modifier.padding(20.dp)) {
            Column(modifier = Modifier.fillMaxWidth().fillMaxHeight(),
                verticalArrangement = Arrangement.Center,
                horizontalAlignment = Alignment.CenterHorizontally) {
                Image(
                    modifier = Modifier.width(100.dp).height(100.dp),
                    imageVector = vectorResource(Res.drawable.baseline_block_24),
                    contentDescription = ""
                )
                Text(stringResource(Res.string.error_no_network), style = MaterialTheme.typography.h5)
            }
            Button(modifier = Modifier.align(Alignment.BottomStart).fillMaxWidth(), onClick = { reduce(RetryButtonClicked) }) {
                Text(stringResource(Res.string.retry))
            }
        }
    }

    @Composable
    fun AutomationBox(modifier: Modifier = Modifier, content: @Composable ColumnScope.() -> Unit) =
        Column(modifier = modifier
            .fillMaxWidth()
            .padding(20.dp)
            .clip(RoundedCornerShape(5.dp))
            .background(Color.LightGray)
            .padding(20.dp),
            content = content)

    @Composable
    private fun Loader() {
        Row {
            LinearProgressIndicator(modifier = Modifier
                .fillMaxWidth()
                .wrapContentHeight())
        }
    }
}