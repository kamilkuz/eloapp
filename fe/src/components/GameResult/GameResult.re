open EloTypes;
open Svc;
open Js.Promise;
open BsReactstrap;

[%bs.raw {|require('./GameResult.scss')|}];

type saveStateType =
  | NOTHING
  | SAVING
  | WARNING(string)
  | FAILURE
  | SUCCESS(int);

type state = {
  userWinnerCode: string,
  userLooserCode: string,
  saveState: saveStateType,
};

type action =
  | ChangeWinUser(string)
  | ChangeLooseUser(string)
  | UpdateClick
  | SetSaveSuccess(int)
  | SetWarning(string)
  | SetFailure
  | ClearSaveState;

type updateRatingsResult =
  | SUCCESS(int)
  | WARNING(string);

exception IllegalCombinationInUpdateRatingsResult;

let component = ReasonReact.reducerComponent("GameResult");

let initialState = () => {
  userWinnerCode: "",
  userLooserCode: "",
  saveState: NOTHING,
};

module GameResponseContentDecoder = {
  type content = int;
  let contentKey = "ratingDiff";

  let decode = Json.Decode.int;
};

module GameResponseDecoder =
  ResponseDecoder.MakeWithWarningsOrContent(GameResponseContentDecoder);

let onSuccess = (containterSend, send, json) => {
  let updateRatingsResult = GameResponseDecoder.decode(json);

  switch (updateRatingsResult) {
  | SUCCESS(ratingDiff) =>
    send(SetSaveSuccess(ratingDiff));
    containterSend(GetUsersSvc);
  | WARNING(msg) => send(SetWarning(msg))
  };
};

let onError = (send, err) => {
  send(SetFailure);
  Js.Console.error(err);
};

let updateRatingsSvc = (state, containterSend) => {
  let winnerLooserCodes = {
    winnerUserCode: String.trim(state.userWinnerCode),
    looserUserCode: String.trim(state.userLooserCode),
  };

  ReasonReact.UpdateWithSideEffects(
    {...state, saveState: SAVING},
    ({send}) => {
      let payload = EncodeUpdateRatings.encode(winnerLooserCodes);
      svcPut("users/update_ratings", payload)
      |> then_(json => onSuccess(containterSend, send, json) |> resolve)
      |> catch(err => onError(send, err) |> resolve)
      |> ignore;
    },
  );
};

let sendWarning = (msg: string) =>
  ReasonReact.SideEffects(({send}) => send(SetWarning(msg)));

let reducer = (containterSend, action, state) =>
  switch (action) {
  | ChangeWinUser(value) =>
    ReasonReact.Update({...state, userWinnerCode: value})
  | ChangeLooseUser(value) =>
    ReasonReact.Update({...state, userLooserCode: value})
  | UpdateClick => updateRatingsSvc(state, containterSend)
  | SetSaveSuccess(ratingDiff) =>
    ReasonReact.Update({
      userWinnerCode: "",
      userLooserCode: "",
      saveState: SUCCESS(ratingDiff),
    })
  | SetWarning(msg) =>
    ReasonReact.Update({...state, saveState: WARNING(msg)})
  | SetFailure => ReasonReact.Update({...state, saveState: FAILURE})
  | ClearSaveState => ReasonReact.Update({...state, saveState: NOTHING})
  };

let valueFromEvent = event => ReactEvent.Form.target(event)##value;

let hanldeDismissAlert = (send, ()) => send(ClearSaveState);

let make = (~containterSend, ~disable, _children) => {
  ...component,
  initialState,
  reducer: reducer(containterSend),
  render: ({state, send}) =>
    <div className="gameResult">
      <div className="messageContainer">
        {
          switch (state.saveState) {
          | SUCCESS(ratingDiff) =>
            <Alert color="success" toggle={hanldeDismissAlert(send)}>
              <span className="success-label">
                {"Updated, rating diff: " |> ReasonReact.string}
              </span>
              <span className="success-rating-diff">
                {string_of_int(ratingDiff) |> ReasonReact.string}
              </span>
            </Alert>
          | WARNING(msg) =>
            <Alert color="warning" toggle={hanldeDismissAlert(send)}>
              {msg |> ReasonReact.string}
            </Alert>
          | SAVING => <LoadingMask />
          | FAILURE => <FailureMask />
          | NOTHING => ReasonReact.null
          }
        }
      </div>
      <Form
        onSubmit={
          event => {
            event |> ReactEvent.Form.preventDefault;
            send(UpdateClick);
          }
        }>
        <Container>
          <Row>
            <Col>
              <FormGroup>
                <Label for_="winnerCode">
                  {"Winner" |> ReasonReact.string}
                </Label>
                <Input
                  type_="text"
                  id="winnerCode"
                  placeholder="code"
                  value={state.userWinnerCode}
                  onChange={
                    event => send(ChangeWinUser(valueFromEvent(event)))
                  }
                />
              </FormGroup>
            </Col>
            <Col>
              <FormGroup>
                <Label for_="looserCode">
                  {"Looser" |> ReasonReact.string}
                </Label>
                <Input
                  type_="text"
                  id="looserCode"
                  placeholder="code"
                  value={state.userLooserCode}
                  onChange={
                    event => send(ChangeLooseUser(valueFromEvent(event)))
                  }
                />
              </FormGroup>
            </Col>
            <Col className="colButton">
              <FormGroup>
                <Button
                  color="primary"
                  disabled={state.saveState === SAVING || disable}>
                  {ReasonReact.string("Update")}
                </Button>
              </FormGroup>
            </Col>
          </Row>
        </Container>
      </Form>
    </div>,
};